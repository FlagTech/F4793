#ifndef QENCODER_2_H
#define QENCODER_2_H

#include <Arduino.h>
#include <FunctionalInterrupt.h>  // 必須引用這個標頭檔

class QEncoder2 {  // 宣告「正交脈衝訊號編碼器」類別
 public:   // 開始宣告公用成員
  // 建構式，必須設定輸入腳位。
  QEncoder2(uint8_t c1, uint8_t c2) : C1_PIN(c1), C2_PIN(c2){};
  void begin();     // 初始化編碼器接腳與中斷常式
  int16_t value();  // 傳回計數值
  bool changed();   // 傳回計數值是否改變
  void swap();      // 翻轉正反轉
  void clear();     // 清除計數值

  ~QEncoder2() {
    detachInterrupt(C1_PIN);  // 解除C1_PIN的中斷
    detachInterrupt(C2_PIN);  // 解除C2_PIN的中斷
  }

 private:   // 開始宣告私有成員
  const uint8_t C1_PIN;            // 儲存輸入腳位的常數
  const uint8_t C2_PIN;            // 儲存輸入腳位的常數
  volatile int16_t count = 0;      // 儲存脈衝數
  volatile int16_t lastCount = 0;  // 儲存「上次」脈衝數
  bool swapped = false;            // 是否翻轉，預設「否」。
  void ARDUINO_ISR_ATTR ISR_C1();  // 中斷處理常式
  void ARDUINO_ISR_ATTR ISR_C2();  // 中斷處理常式
};
#endif
