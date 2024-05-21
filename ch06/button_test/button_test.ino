#include "button.hpp"  // 引用程式庫
#define BTN_PIN 27  // 按鍵接腳

Button btn(BTN_PIN);  // 建立按鍵物件

void setup() {
  Serial.begin(115200);
  btn.begin();  // 初始化按鍵物件
}

void loop() {
  if (btn.changed()) {  // 按鍵被按下了嗎？
    Serial.println("按鍵被按下了！");
  }
}
