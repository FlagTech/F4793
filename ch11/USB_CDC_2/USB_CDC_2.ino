#define RGB_PIN 48   // 自訂全彩LED的控制腳

void setup() {
  Serial0.begin(115200);  // 初始化UART序列埠連線
  Serial.begin(115200);    // 初始化USB CDC序列埠連線
  
  while (!Serial) {    // 等待建立序列連線
    delay(10);           // 讓時間給FreeRTOS處理其他任務
  }
  Serial.println("hello");  // 透過USB CDC輸出初始訊息
}

void loop() {
  Serial0.println("輸出到UART的訊息");
  Serial.println("輸出到USB-CDC的訊息");
  neopixelWrite(RGB_PIN, 60, 60, 60); //亮度60、白色
  delay(1000);
  neopixelWrite(RGB_PIN, 0, 0, 0); // 關燈 / 黑色
  delay(1000);
}
