#ifndef web_ui_h
#define web_ui_h

#include <Arduino.h>

// Common Text Definitions
#define WEB_UI_TITLE "ESP32 Firmware Uploader"
#define WEB_UI_LOGO_TEXT "ESP32FwUploader"
#define WEB_UI_SUBTITLE_TEXT "Over-The-Air Update System"

// External variable for dark mode state
extern bool _webui_dark_mode;

// Light mode color definitions
extern const char* LIGHT_BACKGROUND_COLOR;
extern const char* LIGHT_CONTAINER_BACKGROUND_COLOR;
extern const char* LIGHT_TEXT_COLOR;
extern const char* LIGHT_LOGO_COLOR;
extern const char* LIGHT_PRIMARY_BUTTON_COLOR;
extern const char* LIGHT_PRIMARY_BUTTON_TEXT_COLOR;
extern const char* LIGHT_UPLOAD_AREA_HOVER_COLOR;
extern const char* LIGHT_UPLOAD_AREA_DRAGOVER_COLOR;
extern const char* LIGHT_FILE_INFO_BACKGROUND_COLOR;
extern const char* LIGHT_FILE_INFO_TEXT_COLOR;

// Dark mode color definitions
extern const char* DARK_BACKGROUND_COLOR;
extern const char* DARK_CONTAINER_BACKGROUND_COLOR;
extern const char* DARK_TEXT_COLOR;
extern const char* DARK_LOGO_COLOR;
extern const char* DARK_PRIMARY_BUTTON_COLOR;
extern const char* DARK_PRIMARY_BUTTON_TEXT_COLOR;
extern const char* DARK_UPLOAD_AREA_HOVER_COLOR;
extern const char* DARK_UPLOAD_AREA_DRAGOVER_COLOR;
extern const char* DARK_FILE_INFO_BACKGROUND_COLOR;
extern const char* DARK_FILE_INFO_TEXT_COLOR;

// Function to get HTML content based on current mode
const char* getWebUIHTML();

#endif

