#include "ESP32FwUploader.h"

ESP32FwUploaderClass::ESP32FwUploaderClass(){}

void ESP32FwUploaderClass::begin(ESP32FW_WEBSERVER *server){
  _server = server;
  logMessage("ESP32FwUploader library initialized");

  // Web UI endpoint
  _server->on("/update", HTTP_GET, [&](){
    if (_authenticate && !checkAuth()) {
      setError(ESP32FW_ERROR_AUTH_FAILED, "Authentication failed");
      return _server->requestAuthentication();
    }
    logMessage("Serving OTA web interface");
    _server->send(200, "text/html", getWebUIHTML());
  });

  // OTA upload endpoint
  _server->on("/ota/upload", HTTP_POST, [&](){
    if (_authenticate && !checkAuth()) {
      setError(ESP32FW_ERROR_AUTH_FAILED, "Authentication failed during upload");
      return _server->requestAuthentication();
    }
    
    bool success = !Update.hasError();
    String response = success ? "OK" : "FAIL";
    
    if (!success) {
      String errorMsg = "Update failed: ";
      #if defined(ESP8266) || defined(ESP32)
        errorMsg += Update.errorString();
      #else
        errorMsg += "Unknown error";
      #endif
      setError(ESP32FW_ERROR_UPDATE_END_FAILED, errorMsg);
      logError(errorMsg);
    } else {
      logMessage("Update completed successfully");
    }
    
    _server->send(200, "text/plain", response);
    
    // Call end callback
    if (_onEnd) {
      _onEnd(success);
    }
    
    // Auto reboot after successful update
    if (success && _autoReboot) {
      logMessage("Scheduling reboot in 2 seconds");
      _rebootRequested = true;
      _rebootTime = millis() + 2000; // Reboot after 2 seconds
    }
  }, [&](){
    HTTPUpload& upload = _server->upload();
    static size_t totalReceived = 0;
    
    if(upload.status == UPLOAD_FILE_START){
      logMessage("Update started: " + String(upload.filename.c_str()));
      totalReceived = 0;
      _lastError = ESP32FW_ERROR_NONE;
      _lastErrorMessage = "";
      
      // Reset static variables for new upload
      static bool firstWrite = true;
      firstWrite = true;
      
      // Call start callback
      if (_onStart) {
        _onStart();
      }
      
      // Get mode parameter
      String mode = _server->arg("mode");
      ESP32Fw_Mode otaMode = ESP32FW_MODE_FIRMWARE;
      
      if (mode == "filesystem") {
        otaMode = ESP32FW_MODE_FILESYSTEM;
        logMessage("OTA Mode: Filesystem");
      } else {
        logMessage("OTA Mode: Firmware");
      }
      
      // Note: upload.totalSize may be 0 in some cases, so we'll validate during write phase instead
      
      // Start update process
      bool updateStarted = false;
      #if defined(ESP8266)
        if (otaMode == MYOTA_MODE_FILESYSTEM) {
          size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
          if (upload.totalSize > 0 && upload.totalSize > fsSize) {
            setError(ESP32FW_ERROR_FILE_TOO_LARGE, "File too large for filesystem partition");
            return;
          }
          close_all_fs();
          updateStarted = Update.begin(fsSize, U_FS);
        } else {
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if (upload.totalSize > 0 && upload.totalSize > maxSketchSpace) {
            setError(ESP32FW_ERROR_FILE_TOO_LARGE, "File too large for flash partition");
            return;
          }
          updateStarted = Update.begin(maxSketchSpace, U_FLASH);
        }
      #elif defined(ESP32)
        if (otaMode == ESP32FW_MODE_FILESYSTEM) {
          updateStarted = Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
        } else {
          updateStarted = Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
        }
      #endif
      
      if (!updateStarted) {
        String errorMsg = "Failed to begin update: ";
        #if defined(ESP8266) || defined(ESP32)
          errorMsg += Update.errorString();
        #else
          errorMsg += "Unknown error";
        #endif
        setError(ESP32FW_ERROR_UPDATE_BEGIN_FAILED, errorMsg);
        logError(errorMsg);
      }
      
    } else if(upload.status == UPLOAD_FILE_WRITE){
      // First write - validate we have actual data
      static bool firstWrite = true;
      if (firstWrite) {
        firstWrite = false;
        if (upload.currentSize == 0) {
          setError(ESP32FW_ERROR_INVALID_FILE, "No data received in upload");
          logError("No data received in upload");
          Update.abort();
          return;
        }
        logMessage("First chunk received: " + String(upload.currentSize) + " bytes");
      }
      
      size_t written = Update.write(upload.buf, upload.currentSize);
      if(written != upload.currentSize){
        String errorMsg = "Failed to write update data: ";
        #if defined(ESP8266) || defined(ESP32)
          String updateError = Update.errorString();
          if (updateError.length() == 0 || updateError == "No Error") {
            errorMsg += "Write size mismatch (expected: " + String(upload.currentSize) + ", written: " + String(written) + ")";
            errorMsg += ", Free heap: " + String(ESP.getFreeHeap()) + " bytes";
          } else {
            errorMsg += updateError;
          }
        #else
          errorMsg += "Unknown error";
        #endif
        setError(ESP32FW_ERROR_UPDATE_WRITE_FAILED, errorMsg);
        logError(errorMsg);
        Update.abort();
        return;
      } else {
        totalReceived += upload.currentSize;
        
        // Call progress callback
        if (_onProgress) {
          _onProgress(totalReceived, upload.totalSize);
        }
        
        // Log progress periodically
        if (totalReceived % 10240 == 0 || totalReceived == upload.totalSize) { // Every 10KB or at end
          float percentage = (float)totalReceived / upload.totalSize * 100.0;
          logMessage("Upload progress: " + String(percentage, 1) + "% (" + String(totalReceived) + "/" + String(upload.totalSize) + " bytes)");
        }
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){
        logMessage("Update success: " + String(upload.totalSize) + " bytes");
      } else {
        String errorMsg = "Failed to finalize update: ";
        #if defined(ESP8266) || defined(ESP32)
          errorMsg += Update.errorString();
        #else
          errorMsg += "Unknown error";
        #endif
        setError(ESP32FW_ERROR_UPDATE_END_FAILED, errorMsg);
        logError(errorMsg);
      }
    } else if(upload.status == UPLOAD_FILE_ABORTED) {
      logError("Upload aborted");
      setError(ESP32FW_ERROR_NETWORK_ERROR, "Upload was aborted");
    }
  });
}

void ESP32FwUploaderClass::loop(){
  handleReboot();
}

void ESP32FwUploaderClass::setAuth(const char* username, const char* password) {
  _username = username;
  _password = password;
  _authenticate = (_username.length() > 0 && _password.length() > 0);
  logMessage("Authentication " + String(_authenticate ? "enabled" : "disabled"));
}

void ESP32FwUploaderClass::clearAuth() {
  _authenticate = false;
  _username = "";
  _password = "";
  logMessage("Authentication cleared");
}

void ESP32FwUploaderClass::setAutoReboot(bool enable) {
  _autoReboot = enable;
  logMessage("Auto reboot " + String(enable ? "enabled" : "disabled"));
}

void ESP32FwUploaderClass::setDebug(bool enable) {
  _debugEnabled = enable;
  logMessage("Debug logging " + String(enable ? "enabled" : "disabled"));
}

void ESP32FwUploaderClass::setDarkMode(bool enable) {
  extern bool _webui_dark_mode;
  _webui_dark_mode = enable;
  logMessage("Dark mode " + String(enable ? "enabled" : "disabled"));
}

void ESP32FwUploaderClass::onStart(std::function<void()> callback) {
  _onStart = callback;
}

void ESP32FwUploaderClass::onProgress(std::function<void(size_t current, size_t total)> callback) {
  _onProgress = callback;
}

void ESP32FwUploaderClass::onEnd(std::function<void(bool success)> callback) {
  _onEnd = callback;
}

void ESP32FwUploaderClass::onError(std::function<void(ESP32Fw_Error error, const String& message)> callback) {
  _onError = callback;
}

ESP32Fw_Error ESP32FwUploaderClass::getLastError() {
  return _lastError;
}

String ESP32FwUploaderClass::getLastErrorMessage() {
  return _lastErrorMessage;
}

bool ESP32FwUploaderClass::checkAuth() {
  return _server->authenticate(_username.c_str(), _password.c_str());
}

void ESP32FwUploaderClass::handleReboot() {
  if (_rebootRequested && millis() >= _rebootTime) {
    logMessage("Rebooting device...");
    ESP.restart();
  }
}

void ESP32FwUploaderClass::setError(ESP32Fw_Error error, const String& message) {
  _lastError = error;
  _lastErrorMessage = message;
  
  if (_onError) {
    _onError(error, message);
  }
}

void ESP32FwUploaderClass::logMessage(const String& message) {
  if (_debugEnabled) {
    Serial.print("[ESP32FwUploader] ");
    Serial.println(message);
  }
}

void ESP32FwUploaderClass::logError(const String& message) {
  if (_debugEnabled) {
    Serial.print("[ESP32FwUploader ERROR] ");
    Serial.println(message);
  }
}

ESP32FwUploaderClass ESP32FwUploader;


