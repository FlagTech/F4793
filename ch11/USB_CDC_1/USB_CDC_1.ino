void setup() {
  Serial.begin(115200);    // 初始化UART序列埠連線
  Serial.println("hello");  // 透過USB CDC輸出初始訊息
}

void loop() { }
