#include <CAN.h>
#include <DHT.h>  // 引用Adafruit的DHT程式庫
#define CTX_PIN   21
#define CRX_PIN   22
#define DHTPIN    19   // DHT11的資料接腳
#define DHTTYPE DHT11  // 感測器類型是DHT11

DHT dht(DHTPIN, DHTTYPE);  // 建立DHT11物件

// id可有11位元，資料最大8位元組
void canTX(uint16_t id, float num) {
  CAN.beginPacket (id);
  CAN.print(num);
  CAN.endPacket();
  Serial.printf("送出浮點資料：%.2f\n", num);
}

void canRX() {
  int packetSize = CAN.parsePacket();
  if (packetSize) {
    if (CAN.packetRtr()) {
      uint16_t id = CAN.packetId();
      if (id == 0x11) {
        float t = dht.readTemperature();
        canTX(0x11, t);  // 送出溫度
      } else if (id == 0x12) {
        float h = dht.readHumidity();
        canTX(0x12, h);  // 送出濕度
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  CAN.setPins(CRX_PIN, CTX_PIN);   // 指定CAN收發器的接腳

  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);
  } else {
    Serial.println ("CAN初始化完畢");
  }
}

void loop() {
  canRX();
}
