#include <Arduino.h>
// 處理按鍵的自訂類別
#ifndef SW_H
#define SW_H
#define DEBOUNCE_TIME 300  // 彈跳延遲時間

class Button {
  static void swISR();  // 靜態中斷處理常式
  static Button* btn;   // 指向此類別的靜態成員

 public:
  // 建立此物件時要設置開關腳位
  Button(uint8_t swPin) : SW_PIN(swPin){};
  void begin();    // 初始化物件
  bool changed();  // 傳回按鍵狀態改變與否

 private:
  const byte SW_PIN;                // 開關腳位常數
  volatile uint32_t debounceTimer;  // 彈跳延遲計時
  volatile int swPressed = 0;       // 按下次數
  void IRAM_ATTR onIRQ();           // 實際的事件處理常式
};

void Button::begin() {                     // 初始化按鍵
  pinMode(SW_PIN, INPUT_PULLUP);           // 按鍵接腳，上拉電阻。
  attachInterrupt(SW_PIN, swISR, RISING);  // 附加中斷常式
  btn = this;                              // 讓btn靜態成員指向此物件
  debounceTimer = millis();
}

void Button::swISR() {  // 設定中斷處理常式
  btn->onIRQ();
}

bool Button::changed() {
  if (swPressed > 0) {  // 「啟動」鍵被按下了嗎？
    swPressed = 0;      // 清除按鍵紀錄
    return true;
  }

  return false;
}

void IRAM_ATTR Button::onIRQ() {  // 按鍵的中斷處理常式
  if ((millis() - debounceTimer) >= DEBOUNCE_TIME) {
    swPressed++;
    debounceTimer = millis();  // 紀錄觸發時間
  }
}

Button* Button::btn;  // 初始化類別成員btn
#endif
