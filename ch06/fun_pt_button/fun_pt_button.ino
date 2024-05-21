#include <Arduino.h>
#include "button.h"  // 引用程式庫
#define BTN1_PIN 27  // 按鍵1接腳
#define BTN2_PIN 25  // 按鍵2接腳

Button btn1(BTN1_PIN);  // 建立按鍵物件btn1
Button btn2(BTN2_PIN);  // 建立按鍵物件btn2

void IRAM_ATTR btnISR1() {  // 按鍵1的中斷處理常式
  btn1.onIRQ();
}

void IRAM_ATTR btnISR2() {  // 按鍵2的中斷處理常式
  btn2.onIRQ();
}

void setup() {
  Serial.begin(115200);
  btn1.setISR(btnISR1);   // 設置按鍵1中斷程式
  btn2.setISR(btnISR2);   // 設置按鍵2中斷程式
}

void loop() {
  if (btn1.changed()) {  // 按鍵被按下了嗎？
    Serial.println("按鍵1被按下了！");
  }

  if (btn2.changed()) {  // 按鍵被按下了嗎？
    Serial.println("按鍵2被按下了！");
  }
}
