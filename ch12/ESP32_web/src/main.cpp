#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <U8g2lib.h>
#include <WiFi.h>

#include "sw.hpp"  // 引用按鍵類別

float price(float tax, int n) { return n * tax; }

auto price_tax = std::bind(price, 1.05, std::placeholders::_1);

const int8_t LED_PIN = 5;  // 開發板的LED腳
const char *ssid = "你的Wi-Fi名稱";
const char *password = "你的Wi-Fi密碼";

Button btn(27);  // 按鍵在27腳
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(IPAddress ip) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 16);
    u8g2.print(ip.toString());  // 顯示IP位址
  } while (u8g2.nextPage());
}

AsyncWebServer server(80);

/*
 * @brief 閃爍LED
 * @note  預設閃爍2次，間隔500ms。
 * @param times 指定閃爍次數
 * @param interval 間隔毫秒
 */
void blink(int times = 2, int interval = 500) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(interval);
    digitalWrite(LED_PIN, HIGH);
    delay(interval);
  }
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("無法掛載SPIFFS");
    while (1) {
      delay(50);
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("IP位址：");
  Serial.println(WiFi.localIP());  // 顯示IP位址

  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.serveStatic("/img", SPIFFS, "/www/img/");
  server.serveStatic("/favicon.ico", SPIFFS, "/www/favicon.ico");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  btn.begin();                       // 初始化按鍵
  u8g2.begin();                      // 初始化顯示器
  u8g2.setFont(u8g2_font_8x13B_mf);  // 採13像素高字體
  OLED(WiFi.localIP());              // 顯示IP位址

  server.begin();  // 啟動網站伺服器
  Serial.println("HTTP伺服器開工了～");
}

void loop() {
  if (btn.changed()) {  // 「啟動」鍵被按下了嗎？
    blink(3, 500);
  }

  if (Serial.available()) {
    int val = Serial.parseInt();

    if (val > 0) {
      val = constrain(val, 1, 10);
      Serial.printf("閃爍 %d 次\n", val);
      blink(val);
    }
  }
}
