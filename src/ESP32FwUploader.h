#ifndef ESP32FwUploader_h
#define ESP32FwUploader_h

#include <Arduino.h>
#include "web_ui.h"

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <Updater.h>
  #include <FS.h>
  #include <LittleFS.h>
#define ESP32FW_WEBSERVER ESP8266WebServer
  extern "C" uint32_t _FS_start;
  extern "C" uint32_t _FS_end;
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  #include <Update.h>
  #include <FS.h>
  #include <SPIFFS.h>
#define ESP32FW_WEBSERVER WebServer
#endif

// Debug macros
#ifndef ESP32FW_DEBUG
  #define ESP32FW_DEBUG 0
#endif

#if ESP32FW_DEBUG
  #define ESP32FW_DEBUG_PRINT(x) Serial.print("[ESP32FwUploader] "); Serial.println(x)
  #define ESP32FW_DEBUG_PRINTF(x, ...) Serial.printf("[ESP32FwUploader] " x "\n", ##__VA_ARGS__)
#else
  #define ESP32FW_DEBUG_PRINT(x)
  #define ESP32FW_DEBUG_PRINTF(x, ...)
#endif

enum ESP32Fw_Mode {
    ESP32FW_MODE_FIRMWARE = 0,
    ESP32FW_MODE_FILESYSTEM = 1
};

enum ESP32Fw_Error {
    ESP32FW_ERROR_NONE = 0,
    ESP32FW_ERROR_AUTH_FAILED,
    ESP32FW_ERROR_UPDATE_BEGIN_FAILED,
    ESP32FW_ERROR_UPDATE_WRITE_FAILED,
    ESP32FW_ERROR_UPDATE_END_FAILED,
    ESP32FW_ERROR_FILE_TOO_LARGE,
    ESP32FW_ERROR_INVALID_FILE,
    ESP32FW_ERROR_NETWORK_ERROR
};

class ESP32FwUploaderClass{
  public:
    ESP32FwUploaderClass();
    void begin(ESP32FW_WEBSERVER *server);
    void loop();
    void setAuth(const char* username, const char* password);
    void clearAuth();
    void setAutoReboot(bool enable);
    void setDebug(bool enable);
    void setDarkMode(bool enable);
    
    // Callback functions
    void onStart(std::function<void()> callback);
    void onProgress(std::function<void(size_t current, size_t total)> callback);
    void onEnd(std::function<void(bool success)> callback);
    void onError(std::function<void(ESP32Fw_Error error, const String& message)> callback);
    
    // Error handling
    ESP32Fw_Error getLastError();
    String getLastErrorMessage();

  private:
    ESP32FW_WEBSERVER *_server;
    bool _authenticate = false;
    String _username;
    String _password;
    bool _autoReboot = true;
    bool _rebootRequested = false;
    unsigned long _rebootTime = 0;
    bool _debugEnabled = false;
    
    // Error handling
    ESP32Fw_Error _lastError = ESP32FW_ERROR_NONE;
    String _lastErrorMessage = "";
    
    // Callback functions
    std::function<void()> _onStart = nullptr;
    std::function<void(size_t, size_t)> _onProgress = nullptr;
    std::function<void(bool)> _onEnd = nullptr;
    std::function<void(ESP32Fw_Error, const String&)> _onError = nullptr;
    
    bool checkAuth();
    void handleReboot();
    void setError(ESP32Fw_Error error, const String& message);
    void logMessage(const String& message);
    void logError(const String& message);
};

extern ESP32FwUploaderClass ESP32FwUploader;

#endif


