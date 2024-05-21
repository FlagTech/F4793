#include "QEncoder2.h"

void QEncoder2::begin() {  // 初始化編碼器接腳與中斷常式
  pinMode(C1_PIN, INPUT);  // C1腳設成「輸入」模式
  pinMode(C2_PIN, INPUT);

  // 綁定事件處理常式，ISR_C1和ISR_C2是此類別的「私有」成員。
  attachInterrupt(C1_PIN, std::bind(&QEncoder2::ISR_C1, this), CHANGE);
  attachInterrupt(C2_PIN, std::bind(&QEncoder2::ISR_C2, this), CHANGE);
}

void ARDUINO_ISR_ATTR QEncoder2::ISR_C1() {   // 霍爾C1腳中斷處理常式
  if (digitalRead(C1_PIN) == HIGH) {   // 若C1腳是高電位…
    if (digitalRead(C2_PIN) == LOW) {  // 若C2腳是高電位…
      count++;                         // 順時針轉動，增加計數值。
    } else {
      count--;  // 逆時針轉動，減少計數值。
    }
  } else {                             // C1腳是低電位…
    if (digitalRead(C2_PIN) == LOW) {  // 若C2腳是高電位…
      count--;                         // 逆時針轉動，減少計數值。
    } else {
      count++;  // 順時針轉動，增加計數值。
    }
  }
}

void ARDUINO_ISR_ATTR QEncoder2::ISR_C2() {    // 霍爾C2腳中斷處理常式
  if (digitalRead(C2_PIN) == HIGH) {    // 若C2腳是高電位…
    if (digitalRead(C1_PIN) == HIGH) {  // 若C1腳是高電位…
      count++;                          // 順時針轉動，增加計數值。
    } else {
      count--;  // 逆時針轉動，減少計數值。
    }
  } else {                              // C2腳是低電位…
    if (digitalRead(C1_PIN) == HIGH) {  // 若C1腳是高電位…
      count--;                          // 逆時針轉動，減少計數值。
    } else {
      count++;  // 順時針轉動，增加計數值。
    }
  }
}

bool QEncoder2::changed() {  // 查詢計數值是否有變
  if (lastCount != count) {  // 若計數值有變化
    lastCount = count;       // 更新上次的紀錄
    return true;
  }

  return false;
}

int16_t QEncoder2::value() {  // 傳回計數值
  if (swapped) return count * -1;  // 若有「翻轉」訊號，則改變計數正、負值

  return count;
}

void QEncoder2::clear() {  // 清除計數值
  count = 0;
}

void QEncoder2::swap() { swapped = true; }  // 設成「翻轉」編碼值