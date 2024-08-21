// ------------------------------------------------------------
// ライブラリインクルード部 Library include section.
// ------------------------------------------------------------
#include <M5StickCPlus.h>       // M5StickC 用ライブラリ
#include <WiFi.h>               // WiFi制御用ライブラリ
#include <Preferences.h>        // 不揮発性メモリ制御ライブラリ

// ------------------------------------------------------------
// 定数/変数　定義部　Constant / variable definition section.
// ------------------------------------------------------------
Preferences preferences;
const char* ssid = "your_ssid";        // アクセスポイント情報（SSID）を直接記述
const char* password = "your_password";    // アクセスポイント情報（パスワード）を直接記述

// ------------------------------------------------------------
// Setup 関数　Setup function.
// ------------------------------------------------------------
void setup() {
  // M5StickCの初期化と動作設定　Initialization and operation settings of M5StickC.
  M5.begin(); // 開始
  // シリアルコンソールの開始　Start serial console.
  Serial.begin(115200);
  delay(500);

  // Wi-Fiアクセスポイント情報保存
  preferences.begin("AP-info", false); // 名前空間"AP-info"の指定と書き込みモード（false)
  preferences.putString("ssid", ssid); // ssidの値を保存
  preferences.putString("pass", password); // passwordの値を保存
  preferences.end();
 
  // Wi-Fi接続 We start by connecting to a WiFi network
  Serial.println(); // シリアルポート経由でPCのシリアルモニタに出力
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); // Wi-Fi接続開始
  // Wi-Fi接続の状況を監視（WiFi.statusがWL_CONNECTEDになるまで繰り返し
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Wi-Fi接続結果をシリアルモニタへ出力
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
    //Wi-Fiからの切断
    Serial.println("Disconnect from WiFi");
    WiFi.disconnect();

    // Wi-Fi接続の状況を監視（WiFi.statusがWL_DISCONNECTEDになるまで繰り返し
    while (WiFi.status() != WL_DISCONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Wi-Fi切断結果をシリアルモニタへ出力
    Serial.println("");
    Serial.println("WiFi disconnected");
  }
  else {
    //Wi-Fiへの再接続
    Serial.println("Reconnect to WiFi");
    WiFi.begin(ssid, password); // Wi-Fi接続開始
    // Wi-Fi接続の状況を監視（WiFi.statusがWL_CONNECTEDになるまで繰り返し
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Wi-Fi接続結果をシリアルモニタへ出力
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }