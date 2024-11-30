#include <Cdrv8833.h>   // 馬達驅動程式庫
#include <QEncoder2.h>  // 引用自訂類別
#include <U8g2lib.h>    // OLED顯示器程式庫
#include "motorPID.hpp" // 引用自訂PID類別
#define SAMPLE_TIME 50    // 每50ms（1/20秒）檢驗一次
#define WHEEL_DIAM  33.5  // 輪胎直徑（單位:mm）
#define GEAR_RATIO 30     // 齒輪比
#define PPR  14           // 每圈脈衝數
#define WHEEL_PPR (PPR * GEAR_RATIO) // 輪胎轉一圈的脈衝數
#define MOTOR_FREQ 1000  // 馬達的PWM調變頻率
#define BTN_PIN      4    // 開關接腳

float kp = 1.8;
float ki = 0.15;
float kd = 0.015;
uint32_t prevTime = 0;  // 前次時間（微秒）

QEncoder2 encL(25, 32); // 定義「左編碼器」物件
QEncoder2 encR(33, 34); // 定義「右編碼器」物件

Cdrv8833 motorL(18, 19, 0, 20, true);  // 左馬達物件，反轉。
Cdrv8833 motorR(23, 27, 1, 20); // 右馬達物件

uint16_t turnsTarget(float turns) {
  return turns * WHEEL_PPR * SAMPLE_TIME/1000;
}

uint16_t setpoint = turnsTarget(1.0); // 目標脈衝數，預設每秒轉1圈

PID PID_L(setpoint, 1, 0, 0, SAMPLE_TIME);
PID PID_R(setpoint, 1, 0, 0, SAMPLE_TIME);

// 建立OLED顯示器物件
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(int16_t pulseL, int16_t pulseR) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.print("L: " + String(pulseL));
    u8g2.setCursor(0, 44);
    u8g2.print("R: " + String(pulseR));
    u8g2.setCursor(0, 64);
    u8g2.print("SetPoint: " + String(setpoint));
  } while ( u8g2.nextPage() );
}

void readSerial() {   // 讀取序列輸入字串的函式
  float turns = 1.0;  // 圈數
  
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');

    sscanf(msg.c_str(), "%f, %f,%f,%f", &turns, &kp, &ki, &kd);
    setpoint = turnsTarget(turns);   // 「圈數」轉成「脈衝數」
    PID_L.setParams(setpoint, kp, ki, kd);
    PID_R.setParams(setpoint, kp, ki, kd);
    Serial.printf("setpoint=%d, kp= %f, ki= %f, kd= %f\n", setpoint, kp, ki, kd);
  }
}

void run() {
  uint32_t now = millis();  // 取得目前時間
  float pwrL, pwrR;
  int16_t ticksL, ticksR;   // 左右馬達的當前編碼計數值

  if (now - prevTime > SAMPLE_TIME) {
    cli();  // 停用中斷機制
    ticksL = encL.value();  // 讀取編碼值
    ticksR = encR.value();
    encL.clear();         // 清除編碼值
    encR.clear();
    sei(); // 啟用中斷

    pwrL = PID_L.compute(ticksL);
    pwrR = PID_L.compute(ticksR);
    motorL.move(pwrL);
    motorR.move(pwrR);

    OLED(ticksL, ticksR);
    Serial.printf("ticksL: %ld, pwr: %.2f\t", ticksL, pwrL);
    Serial.printf("ticksR: %ld, pwr: %.2f\n", ticksR, pwrR);

    prevTime = now;
  }
}

void setup() {
  pinMode(BTN_PIN, INPUT);
  Serial.begin(115200);
  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);

  encL.begin();   // 初始化「左編碼器」接腳與中斷處理常式
  encR.begin();   // 初始化「右編碼器」接腳與中斷處理常式
  encR.swap();    // 右側的編碼值要反向

  motorL.setFrequency(MOTOR_FREQ); // 設定馬達的PWM調變頻率
  motorR.setFrequency(MOTOR_FREQ);
  motorL.setDecayMode(drv8833DecayFast);  // 採用快速減模式
  motorR.setDecayMode(drv8833DecayFast);
  motorL.stop();
  motorR.stop();

  prevTime = millis();
}

void loop() {
  static bool start = false;        // 是否為啟動狀態，預設「否」
  static bool lastBtnState = LOW;   // 啟動鍵接腳的前次狀態

  bool btnState = digitalRead(BTN_PIN);  // 讀取按鍵的值
  if (btnState != lastBtnState) {
    // 只有當按鍵狀態為LOW時才翻轉start值
    if (btnState == LOW) {
      start = !start;
    }
  }

  if (start) {   // 若是「啟動」狀態
    run();         // 開始運轉
  } else {
    encL.clear();  // 計數器清零
    encR.clear();  // 計數器清零
    motorL.stop();
    motorR.stop();
  }

  lastBtnState = btnState;  // 更新按鍵狀態
  readSerial();
}
