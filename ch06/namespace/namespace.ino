namespace mac {
  int price = 30;
  void hello() {
    Serial.println("hello");
  }
}

namespace pc {
  int price = 27;
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Mac價格：%d\n", mac::price);
  Serial.printf("PC價格：%d\n", pc::price);
  mac::hello();  // 呼叫位於mac命名空間裡的hello()
}

void loop() { }
