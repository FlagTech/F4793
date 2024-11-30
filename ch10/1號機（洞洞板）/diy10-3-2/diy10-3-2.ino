#include <Cdrv8833.h>
#include <button.hpp>     // 引用按鍵類別
#include <U8g2lib.h>      // OLED顯示器程式庫
#include "bmp.h"          // OLED點陣圖
#define IR_LATCH_PIN 16
#define IR_CLOCK_PIN 17
#define IR_DATA_PIN  36
#define LINE_CENTER  562  // 路徑中間值
#define BTN_PIN      27   // 「啟動鍵」接腳

Button button(BTN_PIN);   // 「啟動鍵」物件
Cdrv8833 motorL(18, 19, 0, 20, true);  // 左馬達物件，反轉。
// Cdrv8833 motorR(26, 23, 1, 20); // 右馬達物件
Cdrv8833 motorR(23, 26, 1, 20); // 右馬達物件

bool start = false;     // 預設「不啟動」

struct pwm_t {
  uint8_t normal = 60;  // 預設（直行）的PWM
  uint8_t slow = 20;    // 低速運作的PWM
  float scale = 0.12;   // 高速PWM的調節比例
} pwm;

uint16_t prevPos = 0;  // 前次感測到的黑線位置

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(int16_t pos, uint8_t left, uint8_t right) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.print("IR: " + String(pos));      // 感測位置
    u8g2.setCursor(0, 44);
    u8g2.print("PWM L: " + String(left));  // 左PWM
    u8g2.setCursor(0, 64);
    u8g2.print("PWM R: " + String(right)); // 右PWM
  } while (u8g2.nextPage());
}

void insert_coin() {   // 顯示「請投幣」點陣圖
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, 128, 64, COIN);
  } while (u8g2.nextPage());
}

// 傳回125~1000，0代表沒有偵測到黑線。
uint16_t checkLine() {
  uint8_t IR_data[8]; // 紀錄黑線位置
  uint8_t cnt = 0;    // 紀錄黑線寬
  float index;        // 索引值
  uint16_t linePos = 0;  // 線條位置值

  digitalWrite(IR_LATCH_PIN, LOW);
  digitalWrite(IR_CLOCK_PIN, LOW);
  digitalWrite(IR_CLOCK_PIN, HIGH);
  digitalWrite(IR_LATCH_PIN, HIGH);

  for (int8_t i = 7; i >= 0 ; i--) {
    digitalWrite(IR_CLOCK_PIN, LOW);
    uint8_t val = digitalRead(IR_DATA_PIN);
    if (val) {
      IR_data[cnt] = i + 1; // 索引範圍：8~1
      cnt ++;
    }
    digitalWrite(IR_CLOCK_PIN, HIGH);
  }

  if (cnt == 0) return 0;   // 沒有感測到黑線…

  if (cnt == 1) {  // 只感應到一點
    index = IR_data[0];  // 取得索引對應的權值
  } else if (cnt % 2 == 0) { // 偶數，取中間兩數的平均值。
    byte middle = cnt / 2;
    index = (IR_data[middle] + IR_data[middle - 1]) / 2.0;
  } else {   // 奇數，直接取中間值。
    byte middle = cnt / 2;
    index = IR_data[middle];
  }
  linePos = index * 1000 / 8;  // 取得索引對應的權值
  return linePos;
}

void run() {
  int16_t lineRaw = checkLine(); // 原始循跡感測值
  int16_t linePos = 0;           // 以0為中心的線條位置
  int fastPWM;

  if (lineRaw == 0) { // 沒有感測到黑線
    linePos = prevPos; // 採用前次感測值
  } else {
    linePos = lineRaw - LINE_CENTER;  // 位置值：-437~438
    prevPos = linePos;
  }

  if (linePos == 0) {  // 此處的0代表「直行」
    motorL.move(pwm.normal);  // 採用預設的速度
    motorR.move(pwm.normal);
    OLED(linePos, pwm.normal, pwm.normal);
  } else if (linePos > 0) {   // 左轉
    fastPWM = pwm.normal + linePos * pwm.scale;       // 高速PWM
    if (fastPWM>100) fastPWM = 100;
    motorL.move(pwm.slow);    // 左輪
    motorR.move(fastPWM);     // 右輪加速
    OLED(linePos, pwm.slow, fastPWM);
  } else {   // 右轉
    fastPWM = pwm.normal - (linePos + 1) * pwm.scale; // linePos要轉正
    if (fastPWM>100) fastPWM = 100;
    motorL.move(fastPWM);     // 左輪加速
    motorR.move(pwm.slow);    // 右輪降速
    OLED(linePos, fastPWM, pwm.slow);
  }
}

void setup() {
  pinMode(IR_LATCH_PIN, OUTPUT);
  pinMode(IR_CLOCK_PIN, OUTPUT);
  pinMode(IR_DATA_PIN,  INPUT);
  
  Serial.begin(115200);

  //motorL.setFrequency(500);  // 設定PWM調變頻率
  //motorR.setFrequency(500);
  motorL.setDecayMode(drv8833DecayFast);  // 採用快速減模式
  motorR.setDecayMode(drv8833DecayFast);
  motorL.stop();
  motorR.stop();

  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);

  button.begin();   // 初始化按鍵
}

void loop() {
  if (button.changed()) {  // 「啟動」鍵被按下了嗎？
    start = 1 - start;  // 等同 start = !start
  }

  if (start) {
    run();   // 啟動自走車
  } else {
    insert_coin();  // 顯示「請投幣」
    motorL.stop();
    motorR.stop();
  }

  delay(5);
}
