void greet(){ 
  Serial.println("hello!");
}

void setup() {
  Serial.begin(115200);

  void (*pt)();  // 宣告函式指標
  pt = greet;  // 儲存greet函式的位址
  (*pt)();     // 執行pt指向的函式，也可寫成pt()
}

void loop() { }
