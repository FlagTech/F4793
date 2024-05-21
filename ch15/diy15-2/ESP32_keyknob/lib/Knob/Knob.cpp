#include "Knob.h"
#define DEBOUNCE_TIME 50  // 彈跳延遲時間

void Knob::begin() {        // 初始化編碼器接腳與中斷常式
  pinMode(CLK_PIN, INPUT);  // 全部接腳都設為「輸入」模式
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
  // 設定中斷常式
  attachInterrupt(CLK_PIN, std::bind(&Knob::rotate, this), CHANGE);
  attachInterrupt(DT_PIN, std::bind(&Knob::rotate, this), CHANGE);
  attachInterrupt(SW_PIN, std::bind(&Knob::ISR_SW, this), CHANGE);

  lastDebounceTime = millis();  // 設定按鍵彈跳延遲計時
}

void ARDUINO_ISR_ATTR Knob::ISR_SW() {  // 按鍵的中斷處理常式
  uint32_t now = millis();
  if ((now - lastDebounceTime) >= DEBOUNCE_TIME) {  // 若彈跳延遲時間已到
    if (digitalRead(SW_PIN) == LOW) {               // 若按鍵被按下
      pressed = true;  // 若按下按鍵，設定「按下」的訊號
    } else {
      released = true;  // 若放開按鍵，設定「放開」的訊號
    }
    lastDebounceTime = now;  // 紀錄觸發時間
  }
}

uint8_t Knob::dialChanged() {  // 查詢計數值是否有變
  uint8_t result = 0;
  if (lastCount != count) {   // 若計數值有變化
    if (lastCount > count) {  // 若計數值有變大
      result = DIR_CCW;       // 傳回順時針旋轉的訊號
    } else {                  // 若計數值有變小
      result = DIR_CW;        // 傳回逆時針旋轉的訊號
    }
    lastCount = count;  // 更新上次的紀錄
  }

  return result;
}

int16_t Knob::dialValue() {  // 傳回計數值
  if (swapped) return count * -1;  // 若有「翻轉」訊號，則改變計數正、負值

  return count;
}

void Knob::clear() {  // 清除計數值
  count = 0;
}

void ARDUINO_ISR_ATTR Knob::rotate() {  // 旋鈕的中斷處理常式
  unsigned char result = process();     // 處理輸入訊號
  if (result == DIR_CW) {
    count++;  // 若有順時針旋轉的訊號，計數值加1
  } else if (result == DIR_CCW) {
    count--;  // 若有逆時針旋轉的訊號，計數值減1
  }
}

uint8_t Knob::process() {  // 處理輸入訊號
  uint8_t pinstate = (digitalRead(CLK_PIN) << 1) | digitalRead(DT_PIN);
  state = ttable[state & 0xf][pinstate];  // 查表
  return state & 0x30;
}

void Knob::swap() { swapped = true; }  // 設成「翻轉」編碼值
