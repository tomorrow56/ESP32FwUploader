# ESP32FwUploader - Arduino OTA Library

A simple Over-The-Air (OTA) update library for ESP32 and ESP8266 microcontrollers with web interface.

## Features

- 3 lines of code to get started
- Responsive web interface
- HTTP Basic Authentication support
- Drag & drop file upload
- Firmware and filesystem OTA updates
- Upload progress tracking
- Error handling and logging
- Callbacks for start, progress, end, and error events
- Auto-reboot after updates

## Supported Platforms

- ESP32 (all variants)
- ESP8266

## Installation

### Arduino IDE

1. Download the library as a ZIP file
2. In Arduino IDE, go to **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded ZIP file
4. The library will be installed and ready to use

### PlatformIO

Add the following to your `platformio.ini` file:

```ini
lib_deps = 
    ESP32FwUploader
```

## Quick Start

### Basic Usage

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

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
  
  // Initialize ESP32FwUploader
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.begin(&server);
  server.begin();
  
  Serial.println("Open http://" + WiFi.localIP().toString() + "/update");
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
}
```

### Advanced Usage with Authentication and Callbacks

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Configure ESP32FwUploader
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.setAuth("admin", "password123");
  ESP32FwUploader.setAutoReboot(true);
  
  // Set up callbacks
  ESP32FwUploader.onStart([]() {
    Serial.println("OTA Update Started!");
  });
  
  ESP32FwUploader.onProgress([](size_t current, size_t total) {
    Serial.printf("Progress: %.1f%%\n", (float)current / total * 100.0);
  });
  
  ESP32FwUploader.onEnd([](bool success) {
    if (success) {
      Serial.println("OTA Update Successful!");
    } else {
      Serial.println("OTA Update Failed!");
    }
  });
  
  ESP32FwUploader.onError([](ESP32Fw_Error error, const String& message) {
    Serial.printf("OTA Error: %s\n", message.c_str());
  });
  
  ESP32FwUploader.begin(&server);
  server.begin();
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
}
```

## API Reference

### Methods

#### `void begin(WebServer* server)`
Initialize the OTA library with a web server instance.

#### `void setAuth(const char* username, const char* password)`
Enable HTTP Basic Authentication for the OTA interface.

#### `void clearAuth()`
Disable authentication.

#### `void setAutoReboot(bool enable)`
Enable or disable automatic reboot after successful updates (default: true).

#### `void setDebug(bool enable)`
Enable or disable debug logging to Serial.

#### `void loop()`
Must be called in the main loop to handle reboot timing.

### Callbacks

#### `void onStart(std::function<void()> callback)`
Called when OTA update starts.

#### `void onProgress(std::function<void(size_t current, size_t total)> callback)`
Called during upload with progress information.

#### `void onEnd(std::function<void(bool success)> callback)`
Called when OTA update ends (success or failure).

#### `void onError(std::function<void(ESP32Fw_Error error, const String& message)> callback)`
Called when an error occurs during the update process.

### Error Handling

#### `ESP32Fw_Error getLastError()`
Get the last error code.

#### `String getLastErrorMessage()`
Get the last error message.

### Error Codes

- `ESP32FW_ERROR_NONE` - No error
- `ESP32FW_ERROR_AUTH_FAILED` - Authentication failed
- `ESP32FW_ERROR_UPDATE_BEGIN_FAILED` - Failed to begin update
- `ESP32FW_ERROR_UPDATE_WRITE_FAILED` - Failed to write update data
- `ESP32FW_ERROR_UPDATE_END_FAILED` - Failed to finalize update
- `ESP32FW_ERROR_FILE_TOO_LARGE` - File too large for partition
- `ESP32FW_ERROR_INVALID_FILE` - Invalid file (zero size)
- `ESP32FW_ERROR_NETWORK_ERROR` - Network error during upload

## Web Interface

The library provides a responsive web interface accessible at `http://your_device_ip/update`. Features include:

- Drag & drop file upload
- Real-time progress bar
- Firmware/Filesystem mode selection
- Mobile design
- Status messages and error reporting

### WebUI Customization

You can customize the appearance and text of the web interface by modifying the `web_ui.h` file.

#### Color Mode Selection

Choose between light and dark themes by uncommenting the desired mode:

```cpp
// WebUI Color Mode Selection
#define WEB_UI_LIGHT_MODE
// #define WEB_UI_DARK_MODE
```

#### Text Customization

Customize the title, logo, and subtitle text:

```cpp
// Common Text Definitions
#define WEB_UI_TITLE "ESP32 Firmware Uploader"
#define WEB_UI_LOGO_TEXT "ESP32FwUploader"
#define WEB_UI_SUBTITLE_TEXT "Over-The-Air Update System"
```

#### Light Mode Colors

When `WEB_UI_LIGHT_MODE` is defined, the following color scheme is used:

```cpp
#define WEB_UI_BACKGROUND_COLOR "linear-gradient(135deg, #667eea 0%, #764ba2 100%)"
#define WEB_UI_CONTAINER_BACKGROUND_COLOR "#FFFFFF"
#define WEB_UI_TEXT_COLOR "#333333"
#define WEB_UI_LOGO_COLOR "#007BFF"
#define WEB_UI_PRIMARY_BUTTON_COLOR "#007BFF"
#define WEB_UI_PRIMARY_BUTTON_TEXT_COLOR "#FFFFFF"
#define WEB_UI_SECONDARY_BUTTON_COLOR "#6C757D"
#define WEB_UI_SECONDARY_BUTTON_TEXT_COLOR "#FFFFFF"
#define WEB_UI_UPLOAD_AREA_HOVER_COLOR "#f8f9ff"
#define WEB_UI_UPLOAD_AREA_DRAGOVER_COLOR "#f0f4ff"
#define WEB_UI_FILE_INFO_BACKGROUND_COLOR "#f8f9fa"
#define WEB_UI_FILE_INFO_TEXT_COLOR "#333333"
```

#### Dark Mode Colors

When `WEB_UI_DARK_MODE` is defined, the following dark color scheme is applied:

```cpp
#define WEB_UI_BACKGROUND_COLOR "linear-gradient(135deg, #2c3e50 0%, #34495e 100%)"
#define WEB_UI_CONTAINER_BACKGROUND_COLOR "#2C3E50"
#define WEB_UI_TEXT_COLOR "#F0F0F0"
#define WEB_UI_LOGO_COLOR "#4A90E2"
#define WEB_UI_PRIMARY_BUTTON_COLOR "#0056B3"
#define WEB_UI_PRIMARY_BUTTON_TEXT_COLOR "#FFFFFF"
#define WEB_UI_SECONDARY_BUTTON_COLOR "#5A6268"
#define WEB_UI_SECONDARY_BUTTON_TEXT_COLOR "#FFFFFF"
#define WEB_UI_UPLOAD_AREA_HOVER_COLOR "#34495e"
#define WEB_UI_UPLOAD_AREA_DRAGOVER_COLOR "#3c5a70"
#define WEB_UI_FILE_INFO_BACKGROUND_COLOR "#3c4a5c"
#define WEB_UI_FILE_INFO_TEXT_COLOR "#F0F0F0"
```

These colors provide appropriate contrast and better visibility in low-light environments.

## File System OTA

To update the file system (SPIFFS/LittleFS), select "Filesystem" mode in the web interface and upload a filesystem image file.

## Security Considerations

- Always use authentication in production environments
- Use strong passwords
- Consider using HTTPS in production (requires additional setup)
- Regularly update credentials

## Troubleshooting

### Common Issues

1. **Upload fails immediately**
   - Check file size (must fit in available partition space)
   - Verify file is a valid firmware/filesystem image
   - Check authentication credentials

2. **Device doesn't reboot after update**
   - Check if auto-reboot is enabled
   - Look for error messages in Serial output
   - Verify update completed successfully

3. **Web interface not accessible**
   - Verify device is connected to WiFi
   - Check IP address in Serial output
   - Ensure web server is started with `server.begin()`

### Debug Mode

Enable debug mode for detailed logging:

```cpp
ESP32FwUploader.setDebug(true);
```

This outputs information about the OTA process to the Serial monitor.

## Examples

See the `examples` folder for working examples:

- `BasicOTA` - Minimal setup
- `AdvancedOTA` - Example with authentication and callbacks

## License

This library is released under the MIT License. See LICENSE file for details.

## Contributing

Submit pull requests or open issues on GitHub.

## Changelog

### Version 1.0.0
- Initial release
- Basic OTA functionality
- Web interface
- Authentication support
- Error handling and logging
- Callback system

