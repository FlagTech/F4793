class Foo {
  public:
  float price(float tax, int n) {  // Foo類別的方法
    return n * tax;
  }
};

Foo f;  // 宣告類別物件“f”

auto price_tax = std::bind(&Foo::price, f, 1.05, std::placeholders::_1);

void setup() {
  Serial.begin(115200);
  Serial.printf("含稅價：%.2f\n", price_tax(10));  // 顯示：“含稅價：10.50”
}

void loop() { }
