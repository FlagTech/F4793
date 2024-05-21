#include <CAN.h>
#include <DHT.h>  // 引用Adafruit的DHT程式庫
#define CTX_PIN   21
#define CRX_PIN   22
#define DHTPIN 19      // DHT11的資料接腳
#define DHTTYPE DHT11  // 感測器類型是DHT11

DHT dht(DHTPIN, DHTTYPE);  // 建立DHT11物件

/*
* 傳送CAN訊息的函式
* 參數id：訊息的識別碼
* 參數num：浮點型態的資料值
*/ 
void canTX(uint16_t id, float num) {
  CAN.beginPacket (id);  // 建立封包、設定識別碼
  CAN.print(num);      // 寫入浮點資料
  CAN.endPacket();
  Serial.printf("送出浮點資料：%.2f\n", num);
}

void canDHT() {
  float t = dht.readTemperature();  // 讀取溫度
  float h = dht.readHumidity();     // 讀取濕度

  canTX(0x11, t); // 傳送溫度封包，自訂識別碼0x11。
  canTX(0x12, h); // 傳送濕度封包，自訂識別碼0x12。
  delay(2000);
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
  canDHT();  // 傳送DHT11溫溼度訊息
}
