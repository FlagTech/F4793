#include <CAN.h>
#define CTX_PIN   21
#define CRX_PIN   22

void onCAN(int packetSize) {
  int id = CAN.packetId();  // 讀取封包的ID

  if (!CAN.packetRtr()) {   // 若不是「請求封包」
    if (id == 0x13) {       // 若ID是0x13
      while (CAN.available()) {
        Serial.print((char) CAN.read());  // 讀取並顯示資料內容
      }
      Serial.println();
    }
  }
}

void setup() {
  Serial.begin (115200);
  CAN.setPins(CRX_PIN, CTX_PIN);   // 指定CAN收發器的接腳

  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);
  } else {
    Serial.println ("CAN初始化完畢");
  }

  CAN.onReceive( onCAN );  // 註冊回呼
}

void loop() { }
