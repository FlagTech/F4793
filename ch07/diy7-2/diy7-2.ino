#include <Cdrv8833.h>
#include <QEncoder2.h>
#include <U8g2lib.h>
#include <button.hpp>   // 引用按鍵類別
#define GEAR_RATIO 30               // 齒輪比
#define GEAR_PPR (GEAR_RATIO * 14)  // 每圈脈衝數
#define ACCEL_TIME_GAP 60           // 加速間隔時間60ms

Button button(27);   // 按鍵在27腳

QEncoder2  enc(33, 34);             // 左編碼器物件
Cdrv8833 motor(18, 19, 0, 0, true); // 左馬達物件，反轉。

drv8833DecayMode decayMode = drv8833DecaySlow;   // 衰減模式
uint16_t pwmFreq = 1000;  // PWM調變頻率
int16_t power = 0;        // PWM工作週期
int8_t powerStep = 1;
float rpm = 0;            // 轉速
bool firstRun = true;   // 是否初次轉動

unsigned long lastTime = 0;
unsigned long lastAccelTime = 0;

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED() {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 16);
    if (decayMode == drv8833DecaySlow) {
      u8g2.print("Slow Decay");  // 慢速衰減
    } else {
      u8g2.print("Fast Decay");  // 慢速衰減
    }
    
    u8g2.setCursor(0, 32);
    u8g2.print(String("FRQ: ") + String(pwmFreq)); // 頻率
    u8g2.setCursor(0, 48);
    u8g2.print(String("PWM: ") + String((power/10.0), 2)); // 小數點後兩位
    u8g2.setCursor(0, 64);
    u8g2.print(String("RPM: ") + String(rpm, 2)); // 每分鐘轉速
  } while (u8g2.nextPage());
}

void readSerial() {   // 讀取序列輸入值
  String txt;         // 暫存序列輸入字串
  uint8_t _decay;     // 暫存序列輸入的衰減模式
  uint16_t _pwmFreq;  // 暫存序列輸入的PWM調變頻率

  if (Serial.available()) {   // 若有序列資料輸入…
    txt = Serial.readStringUntil('\n'); // 讀取字串資料直至'\n'
    sscanf(txt.c_str(), "%d,%d", &_decay, &_pwmFreq);
    Serial.printf("decayMode= %d, pwmFreg= %d\n",
                  _decay, _pwmFreq);

    // 若衰減模式或調變頻率有變
    if ( _decay != (int)decayMode || _pwmFreq != pwmFreq) {
      Serial.println("改了衰減模式或調變頻率，先停止馬達…");
      motor.stop();   // 停止馬達
      power = 0;      // 重設PWM工作週期
      powerStep = 1;  // 重設轉向

      if (_decay)     // 1代表「快速衰減」
        decayMode = drv8833DecayFast;
      else
        decayMode = drv8833DecaySlow;
      
      pwmFreq = _pwmFreq;
      firstRun = true;      // 重新測試初次轉動的PWM
      motor.setFrequency(pwmFreq);    // 改變PWM調變頻率
      motor.setDecayMode(decayMode);  // 改變衰減模式
    }
  }
}

void testMotor() {
  unsigned long currentTime = millis();
  float dutyCycle;
  // 是否過了60ms？
  if (currentTime - lastAccelTime >= ACCEL_TIME_GAP) {
    power += powerStep;
    if (power == 1000 || power == 0) {
      powerStep = -powerStep;
    }
    lastAccelTime = currentTime;  // 紀錄當前毫秒
  }
  dutyCycle = power/10.0;
  
  // Serial.printf("%.2f,%.2f\n", dutyCycle, rpm);  // 輸出PWM和轉速
  motor.move(dutyCycle);  // 轉動馬達
}

void setup() {
  Serial.begin(115200);

  button.begin();   // 初始化按鍵
  
  motor.setFrequency(pwmFreq);  // 改變PWM調變頻率
  motor.setDecayMode(decayMode);
  motor.stop();  // 停止馬達
  
  enc.begin();   // 初始化編碼器
  enc.swap();   // 左側的值要反向

  u8g2.begin();  // 初始化顯示器
  u8g2.setFont(u8g2_font_fub14_tf);  // 採14像素高字體

  lastTime = millis();  // 紀錄當前時間
}

void loop() {
  static bool startTest = false;
  readSerial();  // 讀取序列輸入資料
  
  if (button.changed()) {  // 「啟動」鍵被按下了嗎？
    enc.clear();        // 霍爾編碼器計數器歸零
    startTest = 1 - startTest;

    power = 0;      // 輸出歸零
    powerStep = 1;  // 正轉
    lastAccelTime = millis();  // 紀錄目前毫秒數
  }

  if (startTest) {
    testMotor();    // 測試馬達
  } else {
    motor.stop();   // 停止馬達
  }

  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {  // 每隔1秒執行
    rpm = enc.value() * 60.0 / GEAR_PPR; // 每分鐘轉速

    if (firstRun && rpm > 1) {
      firstRun = false;
      Serial.printf("開始轉動的PWM：%.2f、RPM：%.2f\n", (power/10.0), rpm);
    }

    OLED();       // 顯示轉速
    enc.clear();  // 清除脈衝值
    lastTime = currentTime;
  }
}
