class Foo {
  public:
  float price(float tax, int n) {
    return n * tax;
  }
  // 在類別內部綁定函式
  std::function<float(int)> price_tax = std::bind(&Foo::price, this, 1.05, std::placeholders::_1);
};

Foo f;  // 宣告Foo類別物件f。

void setup() {
  Serial.begin(115200);
  // 執行物件f的price_tax()方法，結果顯示：“含稅價：10.50”。
  Serial.printf("含稅價：%.2f\n", f.price_tax(10));
}

void loop() { }
