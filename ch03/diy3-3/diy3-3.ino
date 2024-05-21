#include <BluetoothSerial.h> // 藍牙序列埠
#include <Preferences.h>
#define THERMO_PIN  36    // 熱敏電阻分壓輸入腳
#define HEATER_PIN  33    // 陶瓷加熱片的 PWM 輸出腳
#define INTERVAL_MS 500   // 0.5 秒
#define PWM_CHANNEL 0     // PWM 通道（0~15）
#define PWM_BITS 8        // 8 位元解析度
#define PWM_FREQ 1000 // PWM 頻率 1KHz
#define ADC_BITS 10   // 類比輸入解析度 10 位元

Preferences prefs;    // 偏好設定物件
BluetoothSerial BT;   // 建立典型藍牙序列通訊物件

struct bt_data {      // 藍牙資料
  char txt[50] = "\0"; // 儲存序列輸入字串
  bool updated = false; // 代表「未更新」
} btData;

float setPoint = 40.0;  // 目標溫度
float kp = 0.0;         // 比例增益
float ki = 0.0;         // 積分增益
float kd = 0.0;          // 微分增益

bool readPrefs() {    // 讀取快閃記憶體
  prefs.begin("PID", true); // true 是「僅讀」
  setPoint = prefs.getFloat("setPoint", 0); // 讀取紀錄
  kp = prefs.getFloat("kp", 0);
  ki = prefs.getFloat("ki", 0);
  kd = prefs.getFloat("kd", 0);
  prefs.end();

  if (setPoint == 0 || kp == 0)
    return false;   // 快閃記憶體沒有紀錄
  return
    true;

}

void writePrefs() {
  prefs.begin("PID", false);    // false 是「可寫」
  prefs.putFloat("setPoint", setPoint); // 寫入快閃記憶體
  prefs.putFloat("kp", kp);
  prefs.putFloat("ki", ki);
  prefs.putFloat("kd", kd);
  prefs.end();
}
float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  int adc = analogRead(THERMO_PIN);
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / ((1 << ADC_BITS) - 1 - adc);

  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

float computePID(float in) {    // 計算 PID
  static float errorSum = 0;    // 累計誤差
  static float prevError = 0; // 前次誤差值
  float error = setPoint - in; // 當前誤差
  float dt = INTERVAL_MS / 1000.0; // 間隔時間（秒）

  errorSum += error * dt; // 積分：累計誤差
  errorSum = constrain(errorSum, 0, 255); // 限制積分範圍

  float errorRate = (error - prevError) / dt; // 微分：誤差程度變化
  prevError = error; // 儲存本次誤差
  // 計算 PID
  float output = kp * error + ki * errorSum + kd * errorRate;
  output = constrain(output, 0, 255); // 限制輸出值範圍
  return output;
}

// 藍牙的回呼函式
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  uint8_t i = 0;
  
  switch (event) {
    case ESP_SPP_OPEN_EVT:
    case ESP_SPP_SRV_OPEN_EVT: // 藍牙主控端已連線
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

void readBT() {
  if (btData.updated) {
    btData.updated = false;

    if (strcmp(btData.txt, "save\n") == 0) {
      BT.println("data saved.");
    } else {
      sscanf(btData.txt, "%f,%f,%f,%f\n", &setPoint, &kp, &ki, &kd);
      BT.printf("setPoint= %.2f, Kp= %.2f, Ki= %.2f, Kd= %.2f\n",
                setPoint, kp, ki, kd);
    }

    memset(btData.txt, 0, sizeof(btData.txt));  // 清空字元陣列
  }
}

// 初始化藍牙
void initBT() {
  if (!BT.begin("ESP32平衡車")) { // 啟用典型藍牙
    Serial.println("無法初始化藍牙通訊，重新啟動ESP32…");
    ESP.restart();
  } else {
    Serial.println("藍牙初始化完畢！");
  }

  BT.register_callback(btCallback);  // 註冊回呼函式
}

void setup() {
  Serial.begin(115200);
  initBT();  // 初始化藍牙

  pinMode(HEATER_PIN, OUTPUT);
  // 設定類比輸入
  analogSetAttenuation(ADC_11db);      // 類比輸入上限3.6V
  analogReadResolution(ADC_BITS);      // 類比輸入的解析度
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_BITS);  // 設置PWM輸出
  ledcAttachPin(HEATER_PIN, PWM_CHANNEL);      // 指定輸出腳
}

void loop() {
  static uint32_t prevTime = 0;   // 前次時間，宣告成「靜態」變數。
  uint32_t now = millis();        // 目前時間
  
  if (now - prevTime >= INTERVAL_MS) {
    prevTime = now;

    float temp = readTemp();          // 讀取溫度
    float power = computePID(temp);   // 計算PID
    ledcWrite(PWM_CHANNEL, (int)power);  // 輸出PID運算值
    Serial.printf("%.2f,%.2f\n", setPoint, temp);
  }

  readBT();
}
