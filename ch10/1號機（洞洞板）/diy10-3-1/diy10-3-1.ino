#include <Cdrv8833.h>  // 馬達驅動程式庫
#include <QEncoder2.h>
#include <U8g2lib.h>    // OLED顯示器程式庫
#define C1_PIN 33      // 編碼器腳位
#define C2_PIN 34
#define WHEEL_DIAM  33.5    // 輪胎直徑（單位:mm）
#define CIRCUM (WHEEL_DIAM * PI)  // 輪胎周長
#define GEAR_RATIO 30       // 齒輪比
#define GEAR_PPR (GEAR_RATIO * 14) // 每圈脈衝數

float pwm = 0;      // 預設PWM值
unsigned long lastTime = 0;  // 暫存上次時間

QEncoder2  enc(C1_PIN, C2_PIN);   // 左編碼器物件

Cdrv8833 motor(18, 19, 0, true); // 左馬達物件，反轉。

// 宣告OLED顯示器控制物件u8g2
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(float rpm, float mms) {  // 接收轉速和速率兩個參數
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 24);
    u8g2.print(String("PWM: ") + String(pwm));
    u8g2.setCursor(0, 44);
    u8g2.print(String("RPM: ") + String(rpm, 2)); // 顯示轉速至小數點後兩位
    u8g2.setCursor(0, 64);
    u8g2.print(String("mm/s: ") + String(mms, 2)); // 每秒移動距離
  } while (u8g2.nextPage());
}

float readSerial() {   // 讀取序列輸入值
  float val = pwm;   // 預設輸入值等同pwm
  while (Serial.available() > 0) {  // 若有序列資料輸入…
    val = Serial.parseFloat();  // 把讀入的字串轉成浮點數
    if (Serial.read() == '\n')  break;   // 讀到‘\n’時退出迴圈
  }
  return val;
}

void setup() {
  Serial.begin(115200);
 
  motor.stop();  // 停止馬達

  enc.begin();   // 初始化編碼器
  enc.swap();   // 感測值要反向
 
  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);  // 設定顯示字體

  lastTime = millis();  // 紀錄當前時間
}

void loop() {
  float num = readSerial();  // 讀取序列輸入資料
  if (num != pwm) {          // 若PWM設定值變了…
    if (num > 100.0) num = 100.0;  // 限制輸入值介於-100~100
    if (num < -100.0) num = -100.0;
    pwm = num;
  }

  motor.move(pwm);  // 轉動馬達
  
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {   // 若經過1秒…
    float rpm = enc.value() * 60.0 / GEAR_PPR; // 每分鐘轉速
    float mms = CIRCUM * rpm / 60;  // 每秒移動距離
    
    OLED(rpm, mms);   // 顯示轉速和移動距離
    enc.clear();    // 清除脈衝值
    lastTime = currentTime;
  }
}
