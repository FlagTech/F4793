#include <BluetoothSerial.h>  // 藍牙序列通訊程式庫
#include <Cdrv8833.h>         // 馬達驅動程式庫
#include <Preferences.h>  // 將資料存入快閃記憶體
#include <button.hpp>             // 按鍵類別
#include <U8g2lib.h>          // OLED顯示器程式庫
#include "bmp.h"          // 「請投幣」點陣圖
#define IR_LATCH_PIN 16   // 74HC166始能腳
#define IR_CLOCK_PIN 17   // 74HC166時脈腳
#define IR_DATA_PIN  36   // 74HC166資料腳
#define LINE_CENTER  562  // 循跡感測中間值
#define INTERVAL     50   // 間隔時間50ms
#define BTN_PIN      27    // 開關接腳

float power = 60.0;  // 預設的直行速度

// PID參數，假設PWM最大增值100，kp→100/438=0.228
float kp = 0.45;  // 從0.15上調到0.5
float ki = 0.015;
float kd = 0;

Button button(BTN_PIN);   // 建立「啟動」鍵物件（腳27或4）

Preferences prefs;  // 宣告儲存偏好設定的物件

BluetoothSerial BT;     // 建立典型藍牙序列通訊物件
struct bt_data {        // 藍牙資料
  char txt[50] = "\0";  // 儲存序列輸入字串
  bool updated = false; // 代表「未更新」
} btData;

// PID相關變數
uint32_t prevT = 0;   // 前次計算PID的時間
double errorSum = 0;  // 累計誤差
double prevError = 0; // 前次誤差值
int16_t linePos = 0;  // 黑線位置

Cdrv8833 motorL(18, 19, 0, 20); // 左馬達物件，反轉。
Cdrv8833 motorR(23, 26, 1, 20, true);

// 建立OLED顯示器物件
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void initBT() {  // 初始化藍牙
  if (!BT.begin("ESP32循線自走車")) { // 啟用典型藍牙
    Serial.println("無法初始化藍牙通訊，重新啟動ESP32…");
    ESP.restart();
  } else {
    Serial.println("藍牙初始化完畢！");
  }

  BT.register_callback(btCallback);  // 註冊回呼函式
}

void readBT() {  // 讀取藍牙資料
  if (btData.updated) {
    btData.updated = false;

    if (strcmp(btData.txt, "save\n") == 0) {
      writePrefs();  // 寫入偏好設定
      BT.println("data saved.");
    } else if (strcmp(btData.txt, "pid\n") == 0) { // 顯示目前的PID參數值
      BT.printf("kp= %f, ki= %f, kd= %f\n", kp, ki, kd);
    } else if (strcmp(btData.txt, "clear\n") == 0) { // 清除偏好設定
      clearPrefs();
      BT.println("prefs cleared.");
    } else {
      sscanf(btData.txt, "%f,%f,%f", &kp, &ki, &kd);
      BT.printf("kp= %f, ki= %f, kd= %f\n", kp, ki, kd);
    }

    memset(btData.txt, 0, sizeof(btData.txt));  // 清空字元陣列
  }
}

// 藍牙的回呼函式
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  uint8_t i = 0;
  switch (event) {
    case ESP_SPP_OPEN_EVT:
    case ESP_SPP_SRV_OPEN_EVT: // 「從機」狀態連線成功，ESP_SPP_OPEN_EVT代表主
      Serial.println("藍牙已連線。");
      break;
    case ESP_SPP_CLOSE_EVT:
      Serial.println("藍牙斷線了！");
      break;
    case ESP_SPP_DATA_IND_EVT:  // 收到輸入資料
      while (BT.available()) {
        btData.txt[i++] = BT.read(); // 儲存每一個輸入字元
      }
      btData.updated = true;
      break;
  }
}

void OLED(float left, float right) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.print("POS: " + String(linePos));  // 感測位置
    u8g2.setCursor(0, 44);
    u8g2.print("L: " + String(left, 2));  // 左PWM
    u8g2.setCursor(0, 64);
    u8g2.print("R: " + String(right, 2));  // 右PWM
  } while (u8g2.nextPage());
}

void insert_coin() {   // 顯示「請投幣」點陣圖
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, 128, 64, COIN);
  } while (u8g2.nextPage());
}

bool readPrefs() { // 從快閃記憶體讀取偏好設定
  bool hasKey = false;
  prefs.begin("PID_LINE", true); // true是「僅讀」
  if (prefs.isKey("kp")) {
    kp  = prefs.getFloat("kp");  // 讀取紀錄
    ki  = prefs.getFloat("ki");
    kd  = prefs.getFloat("kd");
  }
  prefs.end();

  return hasKey;
}

void writePrefs() {  // 寫入偏好設定到快閃記憶體
  prefs.begin("PID_LINE", false);  // 以「可寫」方式開啟
  prefs.getFloat("kp", kp);
  prefs.getFloat("ki", ki);
  prefs.putFloat("kd", kd);
  prefs.end();
}

void clearPrefs() {
  prefs.begin("PID_LINE", false);  // false是「可寫」
  prefs.clear();           // 清除全部鍵、值
  prefs.end();
}

// 傳回125~1000，0代表沒有偵測到黑線。
uint16_t checkLine() {
  uint8_t IR_data[8];   // 紀錄黑線位置
  uint8_t cnt = 0;      // 紀錄黑線寬
  float   middle;        // 中間值
  uint16_t lineRaw = 0; // 感測位置

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

  // 若沒有感測到黑線…
  if (cnt == 0) return 0;

  if (cnt == 1) {  // 只感應到一點
    middle = IR_data[0];  // 取得索引對應的權值
  } else if (cnt % 2 == 0) { // 偶數，取中間兩數的平均值。
    uint8_t index = cnt / 2;
    middle = (IR_data[index] + IR_data[index - 1]) / 2.0;
  } else {   // 奇數，直接取中間值。
    int8_t index = cnt / 2;
    middle = IR_data[index];
  }
  lineRaw = middle * 1000 / 8; // 取得索引對應的權值
  linePos = lineRaw;          // 用於OLED顯示
  return lineRaw;
}

float computePID() {    // 計算PID
  static int16_t preError = 0;  // 前次誤差
  uint16_t pos = checkLine();   // 目前位置
  int16_t error;

  if (pos == 0) {     // 沒有感測到黑線
    error = preError; // 採用前次誤差
  } else {
    error = pos - LINE_CENTER;  // 誤差值：-437~438
    preError = error;
  }

  double dT = (double)INTERVAL / 1000.0;  // 間隔時間轉成秒數（0.05秒）
  errorSum += error * dT;  // 累計誤差
  errorSum = constrain(errorSum, -100, 100);
  // 微分：誤差程度變化
  double errorRate = kd * ((error - prevError) / dT);
  prevError = error;  // 儲存本次誤差
  // Serial.printf("errorRate= %lf\n", errorRate);  // 顯示D值
  // 計算PID
  double output = kp * error + ki * errorSum + errorRate;
  // output = constrain(output, -100, 100);

  return output;
}

void run() {
  uint32_t now = millis();

  if (now - prevT >= INTERVAL) {
    prevT = now;

    float output = computePID(); // 計算PID
    // 傳回負值時，左馬達要轉正。
    float speedL  = constrain(power - output, -100, 100);
    float speedR  = constrain(power + output, -100, 100);

    motorL.move(speedL);  // 驅動馬達
    motorR.move(speedR);
    OLED(speedL, speedR); // 顯示左右速度
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(IR_LATCH_PIN, OUTPUT);
  pinMode(IR_CLOCK_PIN, OUTPUT);
  pinMode(IR_DATA_PIN,  INPUT);

  initBT();                 // 初始化藍牙
  button.begin();         // 初始化按鍵

  bool hasKey = readPrefs();  // 讀取偏好設定值
  if (hasKey) {
    Serial.printf("偏好設定：\n"
                  "kp= %f, ki= %f, kd= %f\n",
                  kp, ki, kd);
  } else {
    Serial.println("偏好設定不存在");
  }

  motorL.setFrequency(500); // 設定PWM調變頻率
  motorR.setFrequency(500);
  motorL.stop();
  motorR.stop();

  u8g2.begin();             // 初始化OLED顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);

  prevT = millis();
}

void loop() {
  static bool start = false;     // 預設「不啟動」
  
  if (button.changed()) {  // 「啟動」鍵被按下了嗎？
    start = 1 - start;  // 等同 start = !start
  }

  if (start) {
    run();   // 啟動自走車
  } else {
    insert_coin(); // 顯示「請投幣」
    motorL.stop();
    motorR.stop();
  }

  readBT();   // 讀取藍牙資料
  delay(5);
}
