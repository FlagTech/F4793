/*
 程式更新以配合ESP32開發環境3.x版
 請參閱：https://swf.com.tw/?p=2000
*/

#define THERMO_PIN 36                 // 熱敏電阻分壓輸入腳
#define HEATER_PIN 33                 // 陶瓷加熱片的 PWM 輸出腳
#define ADC_BITS 10                   // 類比輸入解析度 10 位元
#define INTERVAL_MS (5 * 60 * 1000L)  // 5 分鐘
#define PWM_CHANNEL 0                 // PWM 通道

const uint16_t PWMs[] = { 30, 0, 60, 0, 90, 0 };  // PWM 值
// PWM 值的數量
const uint8_t TOTAL_PWMs = sizeof(PWMs) / sizeof(PWMs[0]);
uint16_t power = 0;  // PWM 的輸出值
uint32_t prevTime;   // 前次時間

// 熱敏電阻值轉換成攝氏溫度值
float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  int adc = analogRead(THERMO_PIN);                    // 讀取熱敏電阻感測值
  float T0 = 25.0 + 273.15;                            // 轉換成溫度值
  float r = (adc * R0) / ((1 << ADC_BITS) - 1 - adc);  // 傳回攝氏溫度
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

void setup() {
  Serial.begin(115200);
  pinMode(HEATER_PIN, OUTPUT);
  power = PWMs[0];  // 讀取第 1 筆 PWM 值
  Serial.printf("PWM：%d", power);
  analogSetAttenuation(ADC_11db);  // 設定類比輸入上限 3.6V
  analogReadResolution(ADC_BITS);  // 設定 ADC（類比轉數位）的解析度

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  // 以下兩行適用於ESP32開發平台3.x版
  ledcAttachChannel(HEATER_PIN, 1000, 8, PWM_CHANNEL);  // 接腳, 頻率, 解析度, 通道
  ledcWrite(HEATER_PIN, power);                         // 開始加熱 (接腳, 工作週期)
#else
  // 以下三行適用於ESP32開發平台2.x版
  ledcSetup(PWM_CHANNEL, 1000, 8);  // 設置 PWM：通道 0、1KHz、8 位元
  ledcAttachPin(HEATER_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, power);  // 開始加熱
#endif
  prevTime = millis();  // 紀錄前次時間
}
void loop() {
  static uint8_t index = 0;  // PWM 陣列的索引

  uint32_t now = millis();
  if (index < TOTAL_PWMs - 1) {
    // 每隔 5 分鐘測量一次溫度，並輸出下一個 PWM 值
    if (now - prevTime >= INTERVAL_MS) {
      prevTime = now;
      power = PWMs[++index];
      float temp = readTemp();  // 取出下一筆 PWM 值
      // 取得感測溫度值
      Serial.printf(", 溫度：%.2f\n", temp);
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      // 底下敘述適用於ESP32開發平台3.x版
      ledcWrite(HEATER_PIN, power);  // 開始加熱
#else
      // 底下敘述適用於ESP32開發平台2.x版
      ledcWrite(PWM_CHANNEL, power); // 開始加熱
#endif
      Serial.printf("PWM：%d", power);
    }
  }
}
