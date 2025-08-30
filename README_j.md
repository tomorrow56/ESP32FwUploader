# ESP32FwUploader - Arduino OTAライブラリ

ESP32およびESP8266マイコン用のWebインターフェースを持つ、シンプルなOTA（Over-The-Air）アップデートライブラリです。

## 機能

- 3行のコードで開始可能
- レスポンシブWebインターフェース
- HTTP Basic認証サポート
- ドラッグ&ドロップファイルアップロード
- ファームウェアとファイルシステムのOTAアップデート
- アップロード進捗表示
- エラーハンドリングとログ機能
- 開始、進捗、終了、エラーイベント用のコールバック
- アップデート後の自動再起動

## サポートプラットフォーム

- ESP32（全バリエーション）
- ESP8266

## インストール

### Arduino IDE

1. ライブラリをZIPファイルとしてダウンロード
2. Arduino IDEで **スケッチ** → **ライブラリをインクルード** → **.ZIP形式のライブラリをインストール**
3. ダウンロードしたZIPファイルを選択
4. ライブラリがインストールされ、使用準備完了

### PlatformIO

`platformio.ini`ファイルに以下を追加：

```ini
lib_deps = 
    ESP32FwUploader
```

## クイックスタート

### 基本的な使用方法

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // WiFiに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFiに接続中...");
  }
  
  Serial.println("WiFi接続完了！");
  Serial.print("IPアドレス: ");
  Serial.println(WiFi.localIP());
  
  // ESP32FwUploaderを初期化
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.setDarkMode(false);  // ライトモードを有効化
  ESP32FwUploader.begin(&server);
  server.begin();
  
  Serial.println("http://" + WiFi.localIP().toString() + "/update を開いてください");
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
}
```

### 認証とコールバックを使用した使用方法

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32FwUploader.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // WiFiに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFiに接続中...");
  }
  
  // ESP32FwUploaderを設定
  ESP32FwUploader.setDebug(true);
  ESP32FwUploader.setDarkMode(false);  // ライトモードを有効化
  ESP32FwUploader.setAuth("admin", "password123");
  ESP32FwUploader.setAutoReboot(true);
  
  // コールバックを設定
  ESP32FwUploader.onStart([]() {
    Serial.println("OTAアップデート開始！");
  });
  
  ESP32FwUploader.onProgress([](size_t current, size_t total) {
    Serial.printf("進捗: %.1f%%\n", (float)current / total * 100.0);
  });
  
  ESP32FwUploader.onEnd([](bool success) {
    if (success) {
      Serial.println("OTAアップデート成功！");
    } else {
      Serial.println("OTAアップデート失敗！");
    }
  });
  
  ESP32FwUploader.onError([](ESP32Fw_Error error, const String& message) {
    Serial.printf("OTAエラー: %s\n", message.c_str());
  });
  
  ESP32FwUploader.begin(&server);
  server.begin();
}

void loop() {
  server.handleClient();
  ESP32FwUploader.loop();
}
```

## APIリファレンス

### メソッド

#### `void begin(WebServer* server)`
Webサーバーインスタンスを使用してOTAライブラリを初期化します。

#### `void setAuth(const char* username, const char* password)`
OTAインターフェースのHTTP Basic認証を有効にします。

#### `void clearAuth()`
認証を無効にします。

#### `void setAutoReboot(bool enable)`
アップデート成功後の自動再起動を有効または無効にします（デフォルト: true）。

#### `void setDebug(bool enable)`
デバッグログをシリアルに出力するかどうかを設定します。

#### `void setDarkMode(bool enable)`
Webインターフェースのカラーモードを設定します（trueでダークモード、falseでライトモード）。

#### `void loop()`
メインループで呼び出す必要があります。アップデート成功後の自動再起動を処理します。

### コールバック

#### `void onStart(std::function<void()> callback)`
OTAアップデート開始時に呼び出されます。

#### `void onProgress(std::function<void(size_t current, size_t total)> callback)`
アップロード中に進捗情報と共に呼び出されます。

#### `void onEnd(std::function<void(bool success)> callback)`
OTAアップデート終了時（成功または失敗）に呼び出されます。

#### `void onError(std::function<void(ESP32Fw_Error error, const String& message)> callback)`
アップデートプロセス中にエラーが発生した時に呼び出されます。

### エラーハンドリング

#### `ESP32Fw_Error getLastError()`
最後のエラーコードを取得します。

#### `String getLastErrorMessage()`
最後のエラーメッセージを取得します。

### エラーコード

- `ESP32FW_ERROR_NONE` - エラーなし
- `ESP32FW_ERROR_AUTH_FAILED` - 認証失敗
- `ESP32FW_ERROR_UPDATE_BEGIN_FAILED` - アップデート開始失敗
- `ESP32FW_ERROR_UPDATE_WRITE_FAILED` - アップデートデータ書き込み失敗
- `ESP32FW_ERROR_UPDATE_END_FAILED` - アップデート完了失敗
- `ESP32FW_ERROR_FILE_TOO_LARGE` - ファイルがパーティションに対して大きすぎる
- `ESP32FW_ERROR_INVALID_FILE` - 無効なファイル（サイズゼロ）
- `ESP32FW_ERROR_NETWORK_ERROR` - アップロード中のネットワークエラー

## Webインターフェース

ライブラリは`http://your_device_ip/update`でアクセス可能なレスポンシブWebインターフェースを提供します。機能には以下が含まれます：

- ドラッグ&ドロップファイルアップロード
- リアルタイム進捗バー
- ファームウェア/ファイルシステムモード選択
- モバイルデザイン
- ステータスメッセージとエラー報告

### WebUIカスタマイズ

`web_ui.h`ファイルを修正することで、Webインターフェースの外観とテキストをカスタマイズできます。

#### 動的カラーモード

ライブラリはAPIメソッドを通じてカラーモード切り替えをサポートしています：

```cpp
ESP32FwUploader.setDarkMode(true);   // ダークモードを有効
ESP32FwUploader.setDarkMode(false);  // ライトモードを有効
```

実行時に内部変数にアクセスしてモードを動的に変更することも可能です：

```cpp
// ダークモード状態の外部変数
extern bool _webui_dark_mode;

// 実行時にモード変更
_webui_dark_mode = true;   // ダークモードに切り替え
_webui_dark_mode = false;  // ライトモードに切り替え
```

#### テキストカスタマイズ

タイトル、ロゴ、サブタイトルのテキストをカスタマイズできます：

```cpp
// Common Text Definitions
#define WEB_UI_TITLE "ESP32 Firmware Uploader"
#define WEB_UI_LOGO_TEXT "ESP32FwUploader"
#define WEB_UI_SUBTITLE_TEXT "Over-The-Air Update System"
```

#### 色定数

配色は`web_ui.h`で宣言され、`web_ui.cpp`で定義されている定数変数を使用しています：

**ライトモードの色：**
```cpp
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
```

**ダークモードの色：**
```cpp
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
```

色をカスタマイズするには、`web_ui.cpp`内の実際の定義を修正してください。これらの定数は異なる照明環境での適切なコントラストと視認性を提供します。

## ファイルシステムOTA

ファイルシステム（SPIFFS/LittleFS）をアップデートするには、Webインターフェースで「Filesystem」モードを選択し、ファイルシステムイメージファイルをアップロードしてください。

## セキュリティに関する考慮事項

- 本番環境では常に認証を使用してください
- 強力なパスワードを使用してください
- 本番環境ではHTTPSの使用を検討してください（追加設定が必要）
- 認証情報を定期的に更新してください

## トラブルシューティング

### よくある問題

1. **アップロードが即座に失敗する**
   - ファイルサイズを確認（利用可能なパーティション領域に収まる必要があります）
   - ファイルが有効なファームウェア/ファイルシステムイメージであることを確認
   - 認証資格情報を確認

2. **アップデート後にデバイスが再起動しない**
   - 自動再起動が有効になっているか確認
   - Serial出力でエラーメッセージを確認
   - アップデートが正常に完了したことを確認

3. **Webインターフェースにアクセスできない**
   - デバイスがWiFiに接続されていることを確認
   - Serial出力でIPアドレスを確認
   - `server.begin()`でWebサーバーが開始されていることを確認

### デバッグモード

デバッグモードを有効にしてログを取得：

```cpp
ESP32FwUploader.setDebug(true);
```

OTAプロセスの情報がSerialモニターに出力されます。

## サンプル

`examples`フォルダのサンプルを参照してください：

- `BasicOTA` - 最小限の設定
- `AdvancedOTA` - 認証とコールバックを含むサンプル

## ライセンス

このライブラリはMITライセンスの下でリリースされています。詳細はLICENSEファイルを参照してください。

## 貢献

GitHubでプルリクエストの送信やイシューの作成をしてください。

## 謝辞

このライブラリは ElegantOTA (https://github.com/ayushsharma82/ElegantOTA) の機能を参考にしています。
コードの修正には manus 及び windsurf を使用しています。

## 変更履歴

### バージョン 1.0.0
- 初回リリース
- 基本的なOTA機能
- Webインターフェース
- 認証サポート
- エラーハンドリングとログ機能
- コールバックシステム
