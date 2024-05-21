#include <button.hpp>   // 引用Button按鍵類別
#include <CAN.h>
#define CTX_PIN   21
#define CRX_PIN   22
#define BTN_PIN   19  // 按鍵的接腳

Button button(BTN_PIN);

void canRX() {
  int packetSize = CAN.parsePacket();
  if (packetSize) {
    uint16_t id = CAN.packetId();

    if (!CAN.packetRtr()) {
      if (id == 0x11 || id == 0x12) {
        float val = 0;
        while (CAN.available()) {
          val = CAN.parseFloat();
        }

        if (id == 0x11)
          Serial.printf("溫度：%.2f°C\n", val);
        if (id == 0x12)
          Serial.printf("濕度：%.2f%%\n", val);
      } else {
        while (CAN.available()) {
          Serial.print((char) CAN.read());
        }
        Serial.println();
      }
    }
  }
}

void setup() {
  Serial.begin (115200);
  button.begin();   // 初始化按鍵

  CAN.setPins (CRX_PIN, CTX_PIN);   // 指定CAN收發器的接腳
  
  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);
  } else {
    Serial.println ("CAN初始化完畢");
  }
  // CAN.filter(0x11, 0x7EE);  // 只會收到0x11
  CAN.filter(0x11, 0x7CC);  // 可收到0x11和0x12
}

void loop() {
  canRX();

  if (button.changed()) {  // 若按鍵被「按下」…
    Serial.println ("送出CAN請求");
    CAN.beginPacket (0x11, 4, true);  // 請求溫度
    CAN.endPacket();

    CAN.beginPacket (0x12, 4, true);  // 請求濕度
    CAN.endPacket();
  }
}
