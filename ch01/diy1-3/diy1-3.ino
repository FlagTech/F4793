#define THERMO_PIN 36  // 熱敏電阻分壓輸入腳
#define HEATER_PIN 33  // 陶瓷加熱片的 PWM 輸出腳 
#define ADC_BITS 10    // 類比輸入解析度 10 位元，最高 12 位元
#define INTERVAL_MS 1000  // 1 秒 
#define PWM_CHANNEL 0     // PWM 通道
float setPoint = 40.0;    // 目標溫度
float baseTemp;           // 儲存基底溫度
float gain = 2.0;         // 增益（PWM 的放大倍率）
int power = 0;            // PWM 輸出值

float delta = 0.155;
float pwmPerDeg = 6.45;

float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  int adc = analogRead(THERMO_PIN);
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / ((1 << ADC_BITS) - 1 - adc);
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

void setup() {
  Serial.begin(115200);
  pinMode(HEATER_PIN, OUTPUT);
  ledcSetup(PWM_CHANNEL, 1000, 8); // 設置 PWM
  ledcAttachPin(HEATER_PIN, PWM_CHANNEL); // 設置 PWM 輸出腳
  analogReadResolution(ADC_BITS);// 設定 ADC 解析度位元

  baseTemp = readTemp(); // 設定初始溫度
  // 計算 PWM 輸出，在其中乘上一個「增益」值
  power = int(gain * (setPoint - baseTemp) * pwmPerDeg);
  power = constrain(power, 0, 255); // 限制 PWM 輸出的範圍
}

void loop() {
  static uint32_t prevTime = 0; // 紀錄前次時間的「靜態」區域變數
  uint32_t now = millis();      // 紀錄當前時刻

  if (now - prevTime >= INTERVAL_MS) { // 若過了 1 秒…
    prevTime = now;
    float temp = readTemp(); // 取得攝氏溫度值

    if (temp < setPoint) {  // 若感測溫度小於目標溫度…
      ledcWrite(PWM_CHANNEL, power);
      Serial.print(power);
    } else {
      ledcWrite(PWM_CHANNEL, 0); // 否則，停止加熱（控制輸出 0）
      Serial.print("0");
    }

    Serial.printf(",%.2f\n", temp); // 向序列埠輸出目前的 PWM 和溫度值
  }
}
