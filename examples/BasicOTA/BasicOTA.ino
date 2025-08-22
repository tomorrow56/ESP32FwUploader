#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Print system information
  Serial.printf("ESP32 Chip model: %s\n", ESP.getChipModel());
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Free sketch space: %d bytes\n", ESP.getFreeSketchSpace());
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize ESP32FwUploader with error callback
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.onError([](ESP32Fw_Error error, const String& message) {
    Serial.printf("OTA Error %d: %s\n", error, message.c_str());
  });
  ESP32FwUploader.begin(&server);
  
  // Start web server
  server.begin();
  Serial.println("Web server started");
  Serial.println("Open http://" + WiFi.localIP().toString() + "/update to access OTA interface");
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
}

