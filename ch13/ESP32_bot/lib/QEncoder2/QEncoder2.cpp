#include "QEncoder2.h"

uint8_t Knob::index = 0;
Knob* Knob::encL;
Knob* Knob::encR;

void Knob::ISR_L() {
  encL->onIRQ ();  
}

void Knob::ISR_R() {
  encR->onIRQ ();  
}

void Knob::begin () {  // 初始化編碼器接腳與中斷常式
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);

  switch (index){
    case 0: 
      attachInterrupt (CLK_PIN, ISR_L, CHANGE);
      encL = this;
      break;
    case 1:
      attachInterrupt (CLK_PIN, ISR_R, CHANGE);
      encR = this;
      break;
  }

  index++;
}

void IRAM_ATTR Knob::onIRQ() {
  if (digitalRead(CLK_PIN) == HIGH) {
    if (digitalRead(DT_PIN) == LOW) {
      count++;
    } else {
      count--;
    }
  } else {
    if (digitalRead(DT_PIN) == LOW) {
      count--;
    } else {
      count++;
    }
  }
}

bool Knob::changed() {
  if (lastCount != count) {  // 若計數值有變化
    lastCount = count;   // 更新上次的紀錄
    return true;
  }

  return false;
}

int16_t Knob::value() {  // 傳回計數值
  if (swapped) 
     return count * -1;
     
  return count; 
}

void Knob::clear() {  // 傳回計數值
  count = 0;
}

void Knob::swap() {
  swapped = true;
}