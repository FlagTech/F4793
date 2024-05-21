#include <CAN.h>
#include <DHT.h>  // 引用Adafruit的DHT程式庫
#define CTX_PIN   21
#define CRX_PIN   22
#define DHTPIN    19   // DHT11的資料接腳
#define DHTTYPE DHT11  // 感測器類型是DHT11

DHT dht(DHTPIN, DHTTYPE);  // 建立DHT11物件

void canRX() {
  int packetSize = CAN.parsePacket();
  if (packetSize) {  // 如果有資料進來…
    uint16_t id = CAN.packetId();  // 取得封包的ID

    if (!CAN.packetRtr()) {   // 確認不是請求封包
      if (id == 0x11 || id == 0x12) {  // 若ID是0x11或0x12
        float val = 0;
        while (CAN.available()) {
          val = CAN.parseFloat();  // 把資料轉換成浮點數字
        }

        if (id == 0x11)
          Serial.printf("溫度：%.2f°C\n", val);
        if (id == 0x12)
          Serial.printf("濕度：%.2f%%\n", val);
      } else {  // 若封包的ID不是0x11或0x12
        while (CAN.available()) {
          Serial.print((char) CAN.read());  // 顯示訊息內容
        }
        Serial.println();
      }
    }
  }
}

void setup() {
  Serial.begin (115200);
  dht.begin();   // 啟動DHT11連線

  CAN.setPins(CRX_PIN, CTX_PIN);   // 指定CAN收發器的接腳
  if (!CAN.begin(500e3)) {         // 嘗試用500Kbps連線
    Serial.println ("CAN初始化失敗～");
    while (1);
  } else {
    Serial.println ("CAN初始化完畢");
  }
}

void loop() {
  canRX();   // 接收CAN訊息
}
