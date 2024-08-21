// セクション1: ライブラリインクルードと定数/変数定義
// ------------------------------------------------------------
// ライブラリインクルード部 Library include section.
// ------------------------------------------------------------
#include <M5StickCPlus.h>   // M5StickC 用ライブラ
#include <WiFi.h>           // Wifi制御用ライブラリ
#include <Preferences.h>    // 不揮発静メモリ制御ライブラリ
#include <time.h>           // 時刻制御用ライブラリ

// ------------------------------------------------------------
// 定数/変数　定義部　Constant / variable definition section.
// ------------------------------------------------------------
Preferences preferences;
const char* ssid = "your_ssid";        // アクセスポイント情報（SSID）を直接記述 Wifi2.4Gに制限されているのに注意
const char* password = "your_password";    // アクセスポイント情報（パスワード）を直接記述

int n_fonts[] = {1,2,4,6,7,8};
int n_color[] = {BLACK,           // 黒
                DARKGREY,         // ダークグレイ
                ORANGE,           // オレンジ
                NAVY,             // ネイビー
                BLUE,             // 青
                GREENYELLOW,      // 黄緑
                DARKGREEN,        // ダークグリーン
                GREEN,            // 緑
                PINK,             // ピンク
                DARKCYAN,         // ダークシアン
                CYAN,             // シアン
                OLIVE,            // オリーブ
                MAROON,           // マルーン（栗色）
                RED,              // 赤
                YELLOW,           // 黄
                PURPLE,           // 紫
                MAGENTA,          // マゼンダ
                LIGHTGREY,        // ライトグレイ
                WHITE};           // 白
const char* n_color_name[] = {"BLACK","DARKGREY", "ORANGE", "NAVY", "BLUE", "GREENYELLOW",
                        "DARKGREEN", "GREEN", "PINK", "DARKCYAN", "CYAN", "OLIVE",
                        "MAROON", "RED", "YELLOW", "PURPLE", "MAGENTA", "LIGHTGREY", "WHITE"};


// NTP接続情報　NTP connection information.
const char* NTPSRV          = "ntp.jst.mfeed.ad.jp";  // NTPサーバーアドレス NTP server address.
const long  GMT_OFFSET      = 9 * 3600;               // GMT-TOKYO(時差９時間）9 hours time difference.
const int   DAYLIGHT_OFFSET = 0;                      // サマータイム設定なし No daylight saving time setting

// 時刻・日付の生成　Time / date ge neration.
RTC_TimeTypeDef RTC_TimeStruct;                    // RTC時刻　Times of Day.
RTC_DateTypeDef RTC_DateStruct;                       // RTC日付  Date
int smin = 0;

// セクション2: 関数定義（時刻同期、時計画面表示、テキスト表示、色表示、図形描画）
// ------------------------------------------------------------
// 時刻同期 関数　Time synchronization function.
// ------------------------------------------------------------
void time_sync(const char* ntpsrv, long gmt_offset, int daylight_offset) {

  // NTPサーバの時間とローカルの時刻を同期　Synchronize NTP server time to local time

  configTime(gmt_offset, daylight_offset, ntpsrv);// NTPの設定

  // Get local time
  struct tm timeInfo;                             // tmオブジェクトをtimeinfoとして生成
  if (getLocalTime(&timeInfo)) {                  // timeinfoに現在時刻を格納
    // 現在時刻の格納が正常終了したら実行
    Serial.print("NTP : ");                       // シリアルモニターに表示
    Serial.println(ntpsrv);                       // シリアルモニターに表示

    // 時刻の取り出し
    RTC_TimeTypeDef TimeStruct;                   // 時刻格納用の構造体を生成
    TimeStruct.Hours   = timeInfo.tm_hour;        // 時を格納
    TimeStruct.Minutes = timeInfo.tm_min;         // 分を格納
    TimeStruct.Seconds = timeInfo.tm_sec;         // 秒を格納
    M5.Rtc.SetTime(&TimeStruct);                  // 時刻の書き込み

    RTC_DateTypeDef DateStruct;                   // 日付格納用の構造体を生成
    DateStruct.WeekDay = timeInfo.tm_wday;        // 曜日を格納
    DateStruct.Month = timeInfo.tm_mon + 1;       // 月（0-11）を格納※1を足す
    DateStruct.Date = timeInfo.tm_mday;           // 日を格納
    DateStruct.Year = timeInfo.tm_year + 1900;    // 年を格納（1900年からの経過年を取得するので1900を足す）
    M5.Rtc.SetDate(&DateStruct);                  // 日付を書き込み

    Serial.printf("RTC %04d-%02d-%02d %02d:%02d:%02d\n",    // シリアルモニターに表示
    DateStruct.Year, DateStruct.Month, DateStruct.Date,
    TimeStruct.Hours, TimeStruct.Minutes, TimeStruct.Seconds);

  }
  else {
    Serial.print("NTP Sync Error ");              // シリアルモニターに表示
  }
}

// ------------------------------------------------------------
// 時計画面の表示用関数　Clock screen display function.
// Clock_screen_display()
// ------------------------------------------------------------
void Clock_screen_display() {

  static const char *_wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"}; // 曜日の定義　Definition of the day of the week.

  // 時刻・日付の取り出し　Extraction of time and date.
  M5.Rtc.GetTime(&RTC_TimeStruct);              // 時間情報の取り出し　Get time information.
  M5.Rtc.GetDate(&RTC_DateStruct);              // 日付情報の取り出し　Get date information.

  // 画面書き換え処理　Screen rewriting process.
  if (smin == RTC_TimeStruct.Minutes) {         // 分単位の変更がかかったかどうか確認
    M5.Lcd.fillRect(180, 40, 190, 60, BLACK);   // 「秒」だけが変わった場合、秒表示エリアだけ書き換え Rewrite only the display area of seconds.
  } else {
    M5.Lcd.fillScreen(BLACK);                   // 「分」が変わったら画面全体を書き換え Rewrite the entire screen when the "minute" changes.
  }

  // ディスプレイに表示する時間の表示色を指定
  M5.Lcd.setTextColor(GREEN);

  // 数字・文字表示部分
  // 時刻表示
  M5.Lcd.setCursor(40, 40, 7);                 //x,y,font 7:48ピクセル7セグ風フォント
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes); // 時分を表示

  // 秒表示
  M5.Lcd.setTextFont(2);                      // 1:Adafruit 8ピクセルASCIIフォント
  M5.Lcd.fillRect(180, 40, 190, 60, BLACK);
  M5.Lcd.printf(":%02d\n", RTC_TimeStruct.Seconds); // 秒を表示

  // 日付表示
  M5.Lcd.setTextColor(WHITE);                 //日付表示文字だけ白色の文字色にする
  M5.Lcd.setCursor(60, 90, 1);                //x,y,font 1:Adafruit 8ピクセルASCIIフォント
  M5.Lcd.printf("Date:%04d.%02d.%02d %s\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date, _wd[RTC_DateStruct.WeekDay]); //曜日を表示

  smin = RTC_TimeStruct.Minutes;              //表示した「分」を保存

  // 上部装飾用　ボックス表示
  M5.Lcd.fillRect(0, 0, 20, 20, RED);
  M5.Lcd.fillRect(20, 0, 40, 20, BLACK);
  M5.Lcd.fillRect(40, 0, 60, 20, RED);
  M5.Lcd.fillRect(60, 0, 80, 20, BLACK);
  M5.Lcd.fillRect(80, 0, 100, 20, RED);
  M5.Lcd.fillRect(100, 0, 120, 20, BLACK);
  M5.Lcd.fillRect(120, 0, 140, 20, RED);
  M5.Lcd.fillRect(140, 0, 160, 20, BLACK);
  M5.Lcd.fillRect(120, 0, 140, 20, RED);
  M5.Lcd.fillRect(140, 0, 160, 20, BLACK);
  M5.Lcd.fillRect(160, 0, 180, 20, RED);
  M5.Lcd.fillRect(180, 0, 200, 20, BLACK);
  M5.Lcd.fillRect(200, 0, 220, 20, RED);
  M5.Lcd.fillRect(220, 0, 240, 20, BLACK);

  // 下部装飾用　ボックス表示
  M5.Lcd.fillRect(0, 115, 20, 135, RED);
  M5.Lcd.fillRect(20, 115, 40, 135, BLACK);
  M5.Lcd.fillRect(40, 115, 60, 135, RED);
  M5.Lcd.fillRect(60, 115, 80, 135, BLACK);
  M5.Lcd.fillRect(80, 115, 100, 135, RED);
  M5.Lcd.fillRect(100, 115, 120, 135, BLACK);
  M5.Lcd.fillRect(120, 115, 140, 135, RED);
  M5.Lcd.fillRect(140, 115, 160, 135, BLACK);
  M5.Lcd.fillRect(120, 115, 140, 135, RED);
  M5.Lcd.fillRect(140, 115, 160, 135, BLACK);
  M5.Lcd.fillRect(160, 115, 180, 135, RED);
  M5.Lcd.fillRect(180, 115, 200, 135, BLACK);
  M5.Lcd.fillRect(200, 115, 220, 135, RED);
  M5.Lcd.fillRect(220, 115, 240, 135, BLACK);

}

// ------------------------------------------------------------
// 標準モジュール（M5.Lcd）を使った画面テキスト表示 関数　
// Screen text display function using standard module (M5.Lcd).
// ------------------------------------------------------------
void display_char_test(int rotate, const char* test_txt) {

  M5.Lcd.setRotation(rotate);

  // フォント種類を変化させて画面に表示する（５秒間隔）
  Serial.println("Change the font type and display it on the screen");         // シリアルモニターに出力
  for(int i = 0; i< sizeof(n_fonts)/sizeof(int); i++) {
    M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
    M5.Lcd.setCursor(0, 0);                             // カーソル位置の指定
    M5.Lcd.setTextFont(n_fonts[i]);                     // フォントの指定
    M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）

    if(n_fonts[i]== 8 ) {
      M5.Lcd.println("1234");                           // 指定テキストの表示  
    }
    else {
      M5.Lcd.println(test_txt);                         // 指定テキストの表示
      M5.Lcd.print(String(M5.Lcd.height()));
      M5.Lcd.print("*");
      M5.Lcd.print(String(M5.Lcd.width()));             // 画面解像度の表示用テキスト生成
    }
    delay(5000);                                        // 待ち
  }
}

// ------------------------------------------------------------
// 標準モジュール（M5.Lcd）を使った画面色表示 関数　
// Screen color display function using standard module (M5.Lcd).
// ------------------------------------------------------------
void display_color_test() {
  // 画面塗りつぶし色を変更して画面に表示する（５秒間隔）
  Serial.println("Change the screen fill color and display it on the screen");  // シリアルモニターに出力
  for(int i = 0; i < sizeof(n_color)/sizeof(int); i++) {
    M5.Lcd.setRotation(3);                              // 画面の向きを指定
    M5.Lcd.fillScreen(n_color[i]);                      // 画面全体の塗りつぶし
    M5.Lcd.fillRect(0, 100, 240, 135, BLACK);           // 画面下部を一部黒色の四角形を描画（カラー名表示用）

    M5.Lcd.setCursor(20, 110);                          // カーソル位置の指定
    M5.Lcd.setTextFont(4);                              // フォントの指定
    M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
    M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
    M5.Lcd.print(n_color_name[i]);                      // 表示しているカラー名を表示

    Serial.println(n_color_name[i]);                    // シリアルモニターに出力

    delay(5000);                                        // 待ち
  }  
}

// ------------------------------------------------------------
// 標準モジュール（M5.Lcd）を使った画面色表示 関数　
// Screen color display function using standard module (M5.Lcd).
// ------------------------------------------------------------
void display_shape_test() {
// 画面塗りつぶし色を変更して画面に表示する（５秒間隔）
Serial.println("Change the screen fill color and display it on the screen");    // シリアルモニターに出力
M5.Lcd.setRotation(3);                              // 画面の向きを指定
// 直線描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("Line");
M5.Lcd.drawLine(0, 68, 240, 68, RED);               // 直線描画（赤）
delay(1000);
M5.Lcd.drawLine(0, 0, 240, 135, BLUE);              // 直線描画（青）
delay(1000);
M5.Lcd.drawLine(0, 135, 240, 0, GREEN);             // 直線描画（緑）
delay(5000);
// 円描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("Circle");
M5.Lcd.drawCircle(10, 10, 10, RED);
delay(1000);
M5.Lcd.drawCircle(50, 30, 30, BLUE);
delay(1000);
M5.Lcd.drawCircle(100, 60, 30, GREEN);
delay(1000);
M5.Lcd.drawCircle(150, 90, 30, YELLOW);
delay(5000);
// 四角形（塗りつぶしなし）描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("Rectangle");
M5.Lcd.drawRect(10, 10, 10, 10, RED);
delay(1000);
M5.Lcd.drawRect(50, 30, 20, 20, BLUE);
delay(1000);
M5.Lcd.drawRect(100, 40, 30, 30, GREEN);
delay(1000);
M5.Lcd.drawRect(150, 60, 30, 30, YELLOW);
delay(5000);
// 円形（塗りつぶしあり）描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("FillCircle");
M5.Lcd.fillCircle(10, 10, 10, RED);
delay(1000);
M5.Lcd.fillCircle(50, 30, 30, BLUE);
delay(1000);
M5.Lcd.fillCircle(100, 60, 30, GREEN);
delay(1000);
M5.Lcd.fillCircle(150, 90, 30, YELLOW);
delay(5000);
// 四角形（塗りつぶしあり）描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("FillRectangle");
M5.Lcd.fillRect(10, 10, 10, 10, RED);
delay(1000);
M5.Lcd.fillRect(50, 30, 20, 20, BLUE);
delay(1000);
M5.Lcd.fillRect(100, 40, 30, 30, GREEN);
delay(1000);
M5.Lcd.fillRect(150, 60, 30, 30, YELLOW);
delay(5000);
// 三角形（塗りつぶしあり）描画
M5.Lcd.fillScreen(BLACK);                           // 画面全体の塗りつぶし
M5.Lcd.setTextFont(4);                              // フォントの指定
M5.Lcd.setTextSize(1);                              // フォントサイズを指定（倍数）
M5.Lcd.setTextColor(WHITE, BLACK);                  // フォントカラーとバックグランドカラーの表示
M5.Lcd.setCursor(100, 20);                          // カーソル位置の指定
M5.Lcd.print("FillTriangle");
M5.Lcd.fillTriangle(15, 5, 0, 15, 30, 15, RED);
delay(1000);
M5.Lcd.fillTriangle(60, 30, 30, 60, 90, 60, BLUE);
delay(1000);
M5.Lcd.fillTriangle(100, 40, 50, 75, 150, 75, GREEN);
delay(1000);
M5.Lcd.fillTriangle(150, 60, 100, 100, 240, 100, YELLOW);
delay(5000);
}
// セクション3: setup関数とloop関数
// ------------------------------------------------------------
// Setup 関数　Setup function.
// ------------------------------------------------------------
void setup() {
// M5StickCの初期化と動作設定　Initialization and operation settings of M5StickC.
M5.begin(); // 開始

//LCD Test
  // M5.begin();
  // M5.Lcd.setRotation(3);
  // M5.Lcd.fillScreen(BLACK);
  // M5.Lcd.setTextColor(WHITE);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.setCursor(0, 0);
  // M5.Lcd.print("Hello, M5StickC Plus!");
  // delay(20000);

// シリアルコンソールの開始　Start serial console.
// Serial.begin(115200);
// delay(500);
M5.Lcd.begin(); // LCDの初期化
// Wi-Fi接続 We start by connecting to a WiFi network
M5.Lcd.println(); // LCDに出力
M5.Lcd.println();
M5.Lcd.setRotation(3);
M5.Lcd.print("Connecting to ");
M5.Lcd.println(ssid);
WiFi.begin(ssid, password); // Wi-Fi接続開始
// Wi-Fi接続の状況を監視（WiFi.statusがWL_CONNECTEDになるまで繰り返し
while (WiFi.status() != WL_CONNECTED) {
delay(500);
M5.Lcd.print("."); }
// Wi-Fi接続結果をLCDへ出力
M5.Lcd.println("");
M5.Lcd.println("WiFi connected");
M5.Lcd.println("IP address: ");
M5.Lcd.println(WiFi.localIP());
// 時刻同期関数
time_sync(NTPSRV, GMT_OFFSET, DAYLIGHT_OFFSET);
// ディスプレイテスト（フォントと画面ローテーション）
for(int i = 0; i <= 3; i++) {
display_char_test(i, "1234567890");
}
// ディスプレイテスト（カラー）
display_color_test();
// ディスプレイテスト（図形描画）
display_shape_test();
}
void loop() {
M5.update();                    // M5状態更新　M5 status update.
Clock_screen_display();         // 時計表示　Clock display.
delay(980);
if (WiFi.status() == WL_CONNECTED) {
//Wi-Fiからの切断
M5.Lcd.println("Disconnect from WiFi");
WiFi.disconnect();
// Wi-Fi接続の状況を監視（WiFi.statusがWL_DISCONNECTEDになるまで繰り返し
while (WiFi.status() !=WL_DISCONNECTED) {
  delay(500);
  M5.Lcd.print(".");
  }

// Wi-Fi切断結果をシリアルモニタへ出力
M5.Lcd.println("");
M5.Lcd.println("WiFi disconnected");
        // 0.98秒待ち
  }
}