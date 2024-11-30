#include <BluetoothSerial.h> // 藍牙序列埠
#include <Cdrv8833.h>
#include <Preferences.h>  // 將資料存入快閃記憶體
#include <U8g2lib.h>      // OLED顯示器程式庫
#include "bmp.h"          // OLED點陣圖
#define IR_LATCH_PIN 16
#define IR_CLOCK_PIN 17
#define IR_DATA_PIN  36
#define LINE_CENTER  562   // 路徑中間值
#define LINE_MAX     438
#define BTN_PIN      4    // 「啟動」鍵的接腳

Preferences prefs;         // 宣告儲存偏好設定的物件

Cdrv8833 motorL(18, 19, 0, 20, true);  // 左馬達物件，反轉。
Cdrv8833 motorR(23, 27, 1, 20); // 右馬達物件

uint16_t prevPos = 0;   // 前次感測到的黑線位置

struct pwm_t {
  uint8_t normal = 60;  // 預設（直行）的PWM
  uint8_t slow = 30;    // 低速運作的PWM
  float scale = 0.09;   // 高速PWM的調節比例
} pwm;

BluetoothSerial BT;   // 建立經典藍牙物件
struct bt_data {      // 藍牙資料
  char txt[50] = "\0";
  bool updated = false;
} btData;

void initBT() {  // 初始化藍牙
  if (!BT.begin("ESP32循跡自走車")) { // 啟用典型藍牙
    Serial.println("無法初始化藍牙通訊，重新啟動ESP32…");
    ESP.restart();
  } else {
    Serial.println("藍牙初始化完畢！");
  }

  BT.register_callback(btCallback);  // 註冊回呼函式
}

void readBT() {  // 讀取藍牙序列資料
  if (btData.updated) {
    btData.updated = false;

    if (strcmp(btData.txt, "save\n") == 0) {
      writePrefs();  // 寫入偏好設定
      BT.println("data saved.");
    } else {
      sscanf(btData.txt, "%hhu,%hhu,%f", &pwm.normal,
            &pwm.slow, &pwm.scale);
      BT.printf("normal= %hhu, slow= %hhu, scale= %f\n",
            pwm.normal, pwm.slow, pwm.scale);
    }

    memset(btData.txt, 0, sizeof(btData.txt));  // 清空字元陣列
  }
}

// 藍牙的回呼函式
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  uint8_t i = 0;
  switch (event) {
    case ESP_SPP_OPEN_EVT:      // 連線成功
    case ESP_SPP_SRV_OPEN_EVT:
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

// 建立OLED顯示器物件
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

void OLED(int16_t pos, uint8_t left, uint8_t right) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.print("IR: " + String(pos));       // 感測位置
    u8g2.setCursor(0, 44);
    u8g2.print("PWM L: " + String(left));   // 左PWM
    u8g2.setCursor(0, 64);
    u8g2.print("PWM R: " + String(right));  // 右PWM
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

bool readPrefs() { // 從快閃記憶體讀取偏好設定
  bool hasKey = false;
  prefs.begin("PWM", true); // true是「僅讀」
  if (prefs.isKey("normal")) {
    pwm.normal  = prefs.getUChar("normal");  // 讀取紀錄
    pwm.slow  = prefs.getUChar("slow");
    pwm.scale  = prefs.getFloat("scale");
  }
  prefs.end();

  return hasKey;
}

void writePrefs() {  // 寫入偏好設定到快閃記憶體
  prefs.begin("PWM", false);  // 以「可寫」方式開啟
  prefs.putUChar("normal", pwm.normal);
  prefs.putUChar("slow", pwm.slow);
  prefs.putFloat("scale", pwm.scale);
  prefs.end();
}

void run() {
  int16_t lineRaw = checkLine(); // 原始循跡感測值
  int16_t linePos = 0;           // 以0為中心的線條位置
  int fastPWM, slowPWM;

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
  Serial.begin(115200);
  pinMode(IR_LATCH_PIN, OUTPUT);
  pinMode(IR_CLOCK_PIN, OUTPUT);
  pinMode(IR_DATA_PIN,  INPUT);
  pinMode(BTN_PIN, INPUT);

  bool hasKey = readPrefs();  // 讀取偏好設定值
  if (hasKey) {
    Serial.printf("偏好設定：\n"
                  "normal= %hhu, slow= %hhu, scale= %f\n",
                  pwm.normal, pwm.slow, pwm.scale);
  } else {
    Serial.println("偏好設定不存在");
  }
  
  initBT();      // 初始化藍牙

  // motorL.setFrequency(500);  // 設定PWM調變頻率
  // motorR.setFrequency(500);
  motorL.setDecayMode(drv8833DecayFast);  // 採用快速減模式
  motorR.setDecayMode(drv8833DecayFast);
  motorL.stop();
  motorR.stop();

  u8g2.begin();  // 啟動顯示器
  u8g2.setFont(u8g2_font_crox4hb_tf);
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
    run();   // 啟動自走車
  } else {
    insert_coin();  // 顯示「請投幣」
    motorL.stop();
    motorR.stop();
  }

  readBT();     // 讀取藍牙序列埠輸入
  lastBtnState = btnState;  // 更新按鍵狀態
  delay(5);
}
