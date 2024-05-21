#include <functional>   // 選擇性地引用包含bind函式定義的程式庫

float price(float tax, int n) {
  return n * tax;
}

auto price_tax = std::bind(price, 1.05, std::placeholders::_1);

void setup() {
  Serial.begin(115200);
  Serial.printf("含稅價：%.2f\n", price_tax(10));  // 僅需傳入「價格」參數
}

void loop() { }
