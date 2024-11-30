/*
 程式更新以配合ESP32開發環境3.x版
 請參閱：https://swf.com.tw/?p=2000
*/

#define THERMO_PIN 36  // 熱敏電阻分壓輸入腳
#define HEATER_PIN 33  // 陶瓷加熱片的 PWM 輸出腳 
#define ADC_BITS 10    // 類比輸入解析度 10 位元，最高 12 位元
#define INTERVAL_MS 1000  // 1 秒 
#define PWM_CHANNEL 0     // PWM 通道
float setPoint = 40.0;    // 目標溫度
float baseTemp;           // 儲存基底溫度
int power = 0;            // PWM 輸出值

float delta = 0.155;
float pwmPerDeg = 6.45;

float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  int adc = analogRead(THERMO_PIN);// 讀取量測溫度
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / ((1 << ADC_BITS) - 1 - adc);
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

void setup() {
  Serial.begin(115200);
  pinMode(HEATER_PIN, OUTPUT);
  
  // 以下兩行適用於ESP32開發平台2.x版
  // ledcSetup(PWM_CHANNEL, 1000, 8); // 設置 PWM 通道、頻率和解析度
  // ledcAttachPin(HEATER_PIN, PWM_CHANNEL); // 設定 PWM 輸出腳
  // 底下敘述適用於ESP32開發平台3.x版
  ledcAttachChannel(HEATER_PIN, 1000, 8, PWM_CHANNEL); // 接腳, 頻率, 解析度, 通道
  // 設定類比轉數位（ADC）的電壓和解析度 
  analogSetAttenuation(ADC_11db); // 類比輸入上限 3.6V，這一行可不寫
  analogReadResolution(ADC_BITS); // ADC 位元解析度
  
  baseTemp = readTemp();          // 讀取並設定基底溫度
  power = int((setPoint - baseTemp) * pwmPerDeg);
  // 底下敘述適用於ESP32開發平台2.x版
  // ledcWrite(PWM_CHANNEL, power);  // 開始加熱
  // 底下敘述適用於ESP32開發平台3.x版
  ledcWrite(HEATER_PIN, power); // 開始加熱
}

void loop() {
  static uint32_t prevTime = 0; // 紀錄前次時間的「靜態」區域變數
  uint32_t now = millis();      // 紀錄當前時刻
  
  if (now - prevTime >= INTERVAL_MS) { // 若過了 1 秒…
    prevTime = now;
    
    float temp = readTemp();
    Serial.printf("%d, %.2f\n", power, temp);
  }
}
