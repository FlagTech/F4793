#include <Cdrv8833.h>     // 馬達驅動程式庫
#include <QEncoder2.h>    // 引用自訂類別
#include <U8g2lib.h>      // OLED顯示器程式庫
#define SAMPLE_TIME 50    // 取樣間隔時間50ms (1/20秒)
#define MOTOR_FREQ  1000  // PWM調變頻率
#define BTN_PIN      4    // 啟動接腳

const int8_t power = 50;      // PWM輸出
const int8_t pwrLimit = power * 0.05 + 1;  // 假設誤差5%
const int8_t pwrMin = power - pwrLimit;    // PWM輸出下限
const int8_t pwrMax = power + pwrLimit;    // PWM輸出上限
uint32_t prevTime = 0;   // 前次時間

QEncoder2 encL(25, 32); // 定義「左編碼器」物件
QEncoder2 encR(33, 34); // 定義「右編碼器」物件

Cdrv8833 motorL(18, 19, 0, 20, true);  // 左馬達物件，反轉。
Cdrv8833 motorR(23, 27, 1, 20); // 右馬達物件

// 建立OLED顯示器物件
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(uint16_t ticksL, uint16_t ticksR, uint16_t pwrL, uint16_t pwrR) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.print("L: " + String(ticksL));
    u8g2.setCursor(0, 30);
    u8g2.print("R: " + String(ticksR));
    u8g2.setCursor(0, 46);
    u8g2.print("PWML: " + String(pwrL));
    u8g2.setCursor(0, 64);
    u8g2.print("PWMR: " + String(pwrR));
  } while ( u8g2.nextPage() );
}

void run() {
  uint32_t now = millis();  // 取得目前時間
  static int16_t pwrL = power;  // 紀錄馬達的PWM輸出
  static int16_t pwrR = power;
  int16_t ticksL, ticksR;  // 左右馬達的當前編碼計數值
  
  if (now - prevTime > SAMPLE_TIME) {
    cli();  // 停用中斷機制
    ticksL = encL.value();  // 讀取編碼值
    ticksR = encR.value();
    encL.clear();   // 清除編碼值
    encR.clear();
    sei(); // 啟用中斷

    if ( ticksL > ticksR ) {  // 若左 > 右編碼…
      pwrL --;   // 減弱左馬達的輸出
      pwrR ++;   // 增加左馬達的輸出
    }

    if ( ticksL < ticksR ) {
      pwrL ++;
      pwrR --;
    }

    pwrL = constrain(pwrL, pwrMin, pwrMax);  // 限制馬達輸出值
    pwrR = constrain(pwrR, pwrMin, pwrMax);

    motorL.move(pwrL);  // 用更新後的輸出值驅動馬達
    motorR.move(pwrR);

    OLED(ticksL, ticksR, pwrL, pwrR);
    prevTime = now;
  }
}

void setup() {
  pinMode(BTN_PIN, INPUT);
  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);  // 設定字體

  encL.begin();   // 初始化「左編碼器」接腳與中斷處理常式
  encR.begin();   // 初始化「右編碼器」接腳與中斷處理常式
  encL.swap();    // 左側的編碼值要反向

  motorL.setFrequency(MOTOR_FREQ); // 設定馬達的PWM調變頻率
  motorR.setFrequency(MOTOR_FREQ);
  motorL.setDecayMode(drv8833DecayFast);  // 採用快速減模式
  motorR.setDecayMode(drv8833DecayFast);
  
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
    motorL.move(power);  // 開始運轉
    motorR.move(power);
    run();
  } else {    
    encL.clear();  // 計數器清零
    encR.clear();
    motorL.stop(); // 停止馬達
    motorR.stop();
  }

  lastBtnState = btnState;  // 更新按鍵狀態
}
