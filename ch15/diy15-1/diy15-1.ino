#include <Arduino.h>
#include <Knob.h>   // 旋轉編碼器程式庫
Knob ko(4, 5, 6);   // 建立旋轉編碼器物件

void setup() {
  Serial.begin(115200);
  ko.begin();   // 初始化旋鈕物件
  // ko.swap(); // 選擇性地翻轉旋鈕值
}

void loop() {
  if (ko.dialChanged()) {
    Serial.printf("旋鈕值:%d\n", ko.dialValue());
  }
  if (ko.pressed) {
    Serial.println("按鍵按下了！");
    ko.pressed = false;  // 清除按鈕按下的訊號
  }
  if (ko.released) {
    Serial.println("按鍵放開了！");
    ko.released = false;  // 清除按鈕放開的訊號
  }
}