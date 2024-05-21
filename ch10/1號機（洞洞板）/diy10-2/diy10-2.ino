#include <button.hpp>   // 引用按鍵類別
#include <U8g2lib.h>    // OLED顯示器程式庫
#include "bmp.h"        // OLED點陣圖

Button button(27);   // 按鍵在27腳
bool start = false;  // 預設「不啟動」

// 建立OLED顯示器物件
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void insert_coin() {   // 顯示「請投幣」點陣圖
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, 128, 64, COIN);  // 繪製點陣圖COIN
  } while (u8g2.nextPage());
}

/*
 參數pos：線條位置
 參數left：左馬達PWM值
 參數right：右馬達PWM值
*/
void OLED(int16_t pos, uint8_t left, uint8_t right) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);         // 設定游標位置
    u8g2.print("IR: " + String(pos));  // 顯示線條位置
    u8g2.setCursor(0, 44);
    u8g2.print("PWM L: " + String(left));  // 左PWM
    u8g2.setCursor(0, 64);
    u8g2.print("PWM R: " + String(right));  // 右PWM
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(115200);

  button.begin();   // 初始化按鍵

  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);
}

void loop() {
  if (button.changed()) {  // 「啟動」鍵被按下了嗎？
    start = 1 - start;  // 等同 start = !start
    Serial.println("按鍵被按下了！");
  }

  if (start) {   // 若「啟動」鍵被按下…
    OLED(100, 23.4, 56.7);     // 顯示虛構的位置和PWM值
  } else {
    insert_coin();  // 顯示「請投幣」
  }

  delay(5);
}
