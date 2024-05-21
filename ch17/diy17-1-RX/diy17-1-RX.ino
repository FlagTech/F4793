#include <CAN.h>
#define CTX_PIN   21
#define CRX_PIN   22

void setup() {
  Serial.begin (115200);
  CAN.setPins(CRX_PIN, CTX_PIN);   // 指定CAN收發器的接腳

  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);
  } else {
    Serial.println ("CAN初始化完畢");
  }
}

void loop() {
  int packetSize = CAN.parsePacket();
  if (packetSize) {
    int id = CAN.packetId();

    if (CAN.packetExtended()) {   // 看看是不是擴充訊框
      Serial.print("這是「擴充訊框」");
    }

    if (!CAN.packetRtr()) {
      if (id == 0x13) {
        while (CAN.available()) {
          Serial.print((char) CAN.read());
        }
        Serial.println();
      }
    }
  }
}
