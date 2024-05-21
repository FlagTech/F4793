#ifndef KNOB_H
#define KNOB_H

#include <Arduino.h>
#include <FunctionalInterrupt.h>  // 處理中斷常式的函式庫

class Knob {  // 宣告「旋轉編碼器」類別
 public:      // 開始宣告公用成員
  // 建構式，必須設定輸入腳位。
  Knob(uint8_t clk, uint8_t dt, uint8_t sw)
      : CLK_PIN(clk), DT_PIN(dt), SW_PIN(sw){};
  static const uint8_t DIR_CW = 0x10;   // 代表順時針旋轉的常數
  static const uint8_t DIR_CCW = 0x20;  // 代表逆時針旋轉的常數
  void begin();                    // 初始化編碼器接腳與中斷常式
  int16_t dialValue();             // 傳回計數值
  uint8_t dialChanged();           // 傳回計數值是否改變
  void swap();                     // 翻轉正反轉
  void clear();                    // 清除計數值
  volatile bool pressed = false;   // 按鈕按下的訊號
  volatile bool released = false;  // 按鈕放開的訊號
  volatile uint8_t state = 0;      // 狀態暫存器

  ~Knob() {                    // 解構式，解除中斷常式
    detachInterrupt(DT_PIN);   // 解除DT_PIN的中斷
    detachInterrupt(CLK_PIN);  // 解除CLK_PIN的中斷
    detachInterrupt(SW_PIN);   // 解除SW_PIN的中斷
  }

 private:                              // 開始宣告私有成員
  const uint8_t CLK_PIN;               // 儲存輸入腳位的常數
  const uint8_t DT_PIN;                // 儲存輸入腳位的常數
  const uint8_t SW_PIN;                // 開關腳的常數
  volatile int16_t count = 0;          // 儲存脈衝數
  volatile int16_t lastCount = 0;      // 儲存「上次」脈衝數
  volatile uint32_t lastDebounceTime;  // 按鍵彈跳延遲計時

  bool swapped = false;  // 是否翻轉旋轉方向，預設「否」。

  uint8_t process();        // 處理輸入訊號
  void ARDUINO_ISR_ATTR ISR_SW();  // 按鍵的中斷處理常式
  void ARDUINO_ISR_ATTR rotate();  // 旋鈕的中斷處理常式

  const uint8_t ttable[7][4] = {  // 狀態表
      {0x0, 0x02, 0x04, 0x0}, {0x03, 0x0, 0x01, 0x0 | 0x10},
      {0x03, 0x02, 0x0, 0x0}, {0x03, 0x02, 0x01, 0x0},
      {0x06, 0x0, 0x04, 0x0}, {0x06, 0x05, 0x0, 0x0 | 0x20},
      {0x06, 0x05, 0x04, 0x0}};
};
#endif
