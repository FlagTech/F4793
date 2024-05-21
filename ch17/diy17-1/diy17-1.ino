#include <CAN.h>
#define CTX_PIN   21  // CAN收發器傳送腳
#define CRX_PIN   22  // CAN收發器接收腳

void setup() {
  Serial.begin (115200);
  CAN.setPins(CRX_PIN, CTX_PIN);  // 指定CAN收發器的接腳

  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);  // 若初始化失敗，程式將停在這裡。
  } else {
    Serial.println ("CAN初始化完畢");
  }
}

void loop() {
  CAN.beginPacket(0x13);
  CAN.print("hello");
  CAN.endPacket();
  delay(2000);
}
