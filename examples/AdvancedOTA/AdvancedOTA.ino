#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
const char* ota_username = "admin";
const char* ota_password = "password123";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize ESP32FwUploader with authentication and callbacks
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.setAuth("admin", "password123");
  ESP32FwUploader.setAutoReboot(true);
  
  // Set up callbacks
  ESP32FwUploader.onStart([]() {
    Serial.println("OTA Update Started!");
  });
  
  ESP32FwUploader.onProgress([](size_t current, size_t total) {
    float progress = (float)current / total * 100.0;
    Serial.printf("Progress: %.1f%% (%u/%u bytes)\n", progress, current, total);
  });
  
  ESP32FwUploader.onEnd([](bool success) {
    if (success) {
      Serial.println("OTA Update Successful!");
    } else {
      Serial.println("OTA Update Failed!");
      Serial.println("Error: " + ESP32FwUploader.getLastErrorMessage());
    }
  });
  
  ESP32FwUploader.onError([](ESP32Fw_Error error, const String& message) {
    Serial.printf("OTA Error %d: %s\n", error, message.c_str());
  });
  
  ESP32FwUploader.begin(&server);
  
  // Add a simple root page
  server.on("/", HTTP_GET, []() {
    String html = "<html><body>";
    html += "<h1>ESP32FwUploader Test Device</h1>";
    html += "<p>Device is running and ready for OTA updates.</p>";
    html += "<p><a href='/update'>Go to OTA Update Page</a></p>";
    html += "<p>WiFi IP: " + WiFi.localIP().toString() + "</p>";
    html += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });
  
  // Start web server
  server.begin();
  Serial.println("Web server started");
  Serial.println("Open http://" + WiFi.localIP().toString() + "/ for device info");
  Serial.println("Open http://" + WiFi.localIP().toString() + "/update for OTA interface");
  Serial.println("Username: " + String(ota_username));
  Serial.println("Password: " + String(ota_password));
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
  
  // Print free heap every 30 seconds
  static unsigned long lastHeapPrint = 0;
  if (millis() - lastHeapPrint > 30000) {
    Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
    lastHeapPrint = millis();
  }
}

