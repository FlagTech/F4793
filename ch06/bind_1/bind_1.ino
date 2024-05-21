// tax參數：浮點稅率值
// n參數：整數價格植
float price(float tax, int n) {
  return n * tax;  // 傳回「含稅價」
}

float price_tax(int n) {   // 輸入價格，傳回含稅價。
  return price(1.05, n);  // 呼叫price()函式
}

void setup() {
  Serial.begin(115200);
  Serial.printf("含稅價：%.2f\n", price_tax(10));
}
void loop() { }
