#ifndef Button_H
#define Button_H
#include <Arduino.h>
#define DEBOUNCE_TIME 300  // 彈跳延遲時間

class Button {
 public:
  Button(uint8_t n) : BTN_PIN(n){};   // 建立此物件時要設置開關腳位
  bool changed();                     // 傳回按鍵狀態改變與否
  void setISR(void (*pt)());          // 設定中斷常式，其參數是函式指標。
  void ARDUINO_ISR_ATTR onIRQ();      // 實際處理中斷的函式

 private:
  const byte BTN_PIN;               // 開關腳位常數
  volatile uint32_t debounceTimer;  // 彈跳延遲計時
  volatile int press = 0;           // 按下次數
};
#endif
