// 處理按鍵的自訂類別
#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include <FunctionalInterrupt.h>
#define DEBOUNCE_TIME 300  // 彈跳延遲時間

class Button {
 public:
  // @brief 建立此物件時要設置開關腳位
  Button(uint8_t n) : BTN_PIN(n) { pinMode(BTN_PIN, INPUT_PULLUP); };
  // @brief 初始化按鍵物件
  void begin() {  // 初始化按鍵
    attachInterrupt(BTN_PIN, std::bind(&Button::onIRQ, this), RISING);
    debounceTimer = millis();
  };

  // @brief 傳回按鍵狀態改變與否
  bool changed() {
    if (press > 0) {  // 「啟動」鍵被按下了嗎？
      press = 0;      // 清除按鍵紀錄
      return true;
    }

    return false;
  }

  ~Button() { detachInterrupt(BTN_PIN); }

 private:
  const byte BTN_PIN;               // 開關腳位常數
  volatile uint32_t debounceTimer;  // 彈跳延遲計時
  volatile uint8_t press = 0;       // 按下次數

  void ARDUINO_ISR_ATTR onIRQ() {  // 按鍵的中斷處理常式
    if ((millis() - debounceTimer) >= DEBOUNCE_TIME) {
      press++;
      debounceTimer = millis();  // 紀錄觸發時間
    }
  }
};

#endif
