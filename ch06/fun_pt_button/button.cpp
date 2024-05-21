#include "button.h"

bool Button::changed() {
  if (press > 0) {  // 按鍵被按下了嗎？
    press = 0;      // 清除按鍵紀錄
    return true;
  }

  return false;
}

void Button::setISR(void (*pt)()) {
  pinMode(BTN_PIN, INPUT_PULLUP);        // 按鍵接腳，上拉電阻。
  attachInterrupt(BTN_PIN, pt, RISING);  // 附加中斷常式
  debounceTimer = millis();
}

void ARDUINO_ISR_ATTR Button::onIRQ() {  // 按鍵的中斷處理常式
  if ((millis() - debounceTimer) >= DEBOUNCE_TIME) {
    press++;
    debounceTimer = millis();  // 紀錄觸發時間
  }
}
