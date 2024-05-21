#ifndef QENCODER_2_H
#define QENCODER_2_H

#include <Arduino.h>

// 宣告「計數器」類別
class Knob {
  public:
    static Knob* encL;  // 編碼器物件0的指標
    static Knob* encR;  // 編碼器物件1的指標
    static void ISR_L();   // 中斷處理常式0
    static void ISR_R();   // 中斷處理常式1
    static uint8_t index;    // 編碼器物件索引

    // 建構式，必須設定輸入腳位。
    Knob(uint8_t c1, uint8_t c2) : 
                CLK_PIN(c1), DT_PIN(c2){};
    void    begin();    // 初始化編碼器接腳與中斷常式
    int16_t value();    // 傳回計數值
    bool    changed();  // 傳回計數值是否改變
    void    swap();     // 翻轉正反轉
    void    clear();    // 清除計數值

  private:
    const uint8_t CLK_PIN;  // 儲存輸入腳位的常數
    const uint8_t DT_PIN;   // 儲存輸入腳位的常數
    volatile int16_t count = 0; // 儲存脈衝數
    int16_t lastCount = 0;  // 儲存「上次」脈衝數
    bool swapped = false;   // 是否翻轉，預設「否」。
    void IRAM_ATTR onIRQ();  // 中斷處理常式
};
#endif