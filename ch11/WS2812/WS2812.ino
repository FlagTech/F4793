#include <Arduino.h>
const uint8_t RGB_PIN = 38;  // 開發板的RGB控制腳，請自行修改。
// 定義LED的RGB色彩
const uint32_t RGBs[] = {0xff6600, 0xff9933, 0x66cc99,
                         0x00cc66, 0x0099ff, 0x9966cc};

uint16_t delay_ms = 1000;   // 延遲毫秒數

void LED_ON(uint8_t index) {  // 從RGBs陣列，取出指定索引的紅、綠、藍色
  uint8_t R = (RGBs[index] >> 16) & 0xff;  // 紅色
  uint8_t G = (RGBs[index] >> 8) & 0xff;   // 綠色
  uint8_t B = RGBs[index] & 0xff;             // 藍色
  neopixelWrite(RGB_PIN, R, G, B); // 令LED呈現指定的R, G, B色彩。
}

void LED_OFF() {  // 熄滅LED；嚴格來說，是令它呈現「黑色」。
neopixelWrite(RGB_PIN, 0, 0, 0);
}

void setup() { 
  Serial.begin(115200); 
}

void loop() {
  static uint8_t index = 0;  // 色彩索引

  LED_ON(index++);
  delay(delay_ms);  // 延遲一下
  LED_OFF();
  delay(delay_ms);  // 延遲一下

  // 取得RGBs陣列大小，若index超過陣列大小，將index歸0
  if (index >= sizeof(RGBs) / sizeof(RGBs[0])) {
    Serial.println("重頭開始！");
    index = 0;
  }
}
