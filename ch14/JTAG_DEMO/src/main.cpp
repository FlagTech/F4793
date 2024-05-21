#include <Arduino.h>
const uint8_t RGB_PIN = 48;  // 開發板的RGB腳，48
const uint32_t RGBs[] = {0xff6600, 0xff9933, 0x66cc99,
                         0x00cc66, 0x0099ff, 0x9966cc};

uint16_t delay_ms = 1000;  // LED閃爍時間間隔，單位ms

void LED_ON(uint8_t index) {
  // 取得指定RGBs陣列，指定索引的紅色
  uint8_t R = (RGBs[index] >> 16) & 0xff;
  uint8_t G = (RGBs[index] >> 8) & 0xff;
  uint8_t B = RGBs[index] & 0xff;
  neopixelWrite(RGB_PIN, R, G, B);
}

void LED_OFF() { neopixelWrite(RGB_PIN, 0, 0, 0); }

void setup() { Serial.begin(115200); }

void loop() {
  // 宣告靜態變數index
  static uint8_t index = 0;

  LED_ON(index++);
  delay(delay_ms);  // 等一下子
  LED_OFF();

  // 取得RGBs陣列大小，若index超過陣列大小，將index歸0
  if (index >= sizeof(RGBs) / sizeof(RGBs[0])) {
    Serial.println("重頭開始！");
    index = 0;
  }
}