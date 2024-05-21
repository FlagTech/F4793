#include <button.hpp>   // 引用Button按鍵類別
#include <QEncoder2.h>  // 引用自訂類別
#define WHEEL_DIAM  33.5  // 輪胎直徑（單位:mm）
#define GEAR_RATIO 30  // 齒輪比
#define PPR  28  // 每圈脈衝數
// (輪胎周長) ÷ (每圈脈衝數 × 齒輪比) 
#define WHEEL_RATIO (WHEEL_DIAM * PI) / (PPR * GEAR_RATIO)

Button button(27);   // 按鍵在27腳
QEncoder2 enc(33, 34); // 編碼器物件
float distance = 0;  // 移動距離

void setup() {
  Serial.begin(115200);
  
  button.begin();   // 初始化按鍵
  enc.swap();       // 翻轉編碼值
  enc.begin();
}

void loop() {
  if (button.changed()) {  // 「啟動」鍵被按下了嗎？
    enc.clear();           // 計數器清零
  }
  
  if(enc.changed()) {  // 若計數值改變了…
    distance = enc.value() * WHEEL_RATIO;  // 計算移動距離
    Serial.printf("脈衝數：%d, 移動距離：%.2fmm\n",  enc.value(), distance);
  }
}
