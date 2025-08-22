# ESP32FwUploader - Arduino OTA Library

A simple and elegant Over-The-Air (OTA) update library for ESP32 and ESP8266 microcontrollers with a beautiful web interface.

## Features

- Easy to use - Just 3 lines of code to get started
- Beautiful responsive web interface
- HTTP Basic Authentication support
- Mobile-friendly drag & drop file upload
- Both firmware and filesystem OTA updates
- Real-time upload progress
- Comprehensive error handling and logging
- Customizable callbacks for start, progress, end, and error events
- Auto-reboot after successful updates

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
  
  // Initialize ESP32FwUploader (just 3 lines!)
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

The library provides a beautiful, responsive web interface accessible at `http://your_device_ip/update`. Features include:

- Drag & drop file upload
- Real-time progress bar
- Firmware/Filesystem mode selection
- Mobile-friendly design
- Status messages and error reporting

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

Enable debug mode to get detailed logging:

```cpp
ESP32FwUploader.setDebug(true);
```

This will output detailed information about the OTA process to the Serial monitor.

## Examples

See the `examples` folder for complete working examples:

- `BasicOTA` - Minimal setup
- `AdvancedOTA` - Full featured example with authentication and callbacks

## License

This library is released under the MIT License. See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues on GitHub.

## Changelog

### Version 1.0.0
- Initial release
- Basic OTA functionality
- Web interface
- Authentication support
- Error handling and logging
- Callback system

