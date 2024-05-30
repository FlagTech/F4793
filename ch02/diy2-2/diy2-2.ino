#define THERMO_PIN 36   // 熱敏電阻分壓輸入腳
#define HEATER_PIN 33   // 陶瓷加熱片的 PWM 輸出腳
#define ADC_BITS 10     // 類比輸入解析度 10 位元
#define PWM_CHANNEL 0   // PWM通道
#define PWM_BITS 8      // 8位元解析度
#define PWM_FREQ 1000   // 1KHz
#define INTERVAL_MS 500 // 間隔時間 500 毫秒

float setPoint = 40.0;  // 目標溫度
float kp = 0.0;         // 比例增益
float ki = 0.0;         // 積分增益
float kd = 0.0;         // 微分增益
float errorSum = 0;     // 累計誤差


void readSerial() {     // 讀取序列輸入字串的函式
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    sscanf(data.c_str(), "%f,%f,%f,%f", &setPoint, &kp, &ki, &kd);
//    Serial.printf("setPoint= %.2f, kp= %.2f, ki= %.2f, kd= %.2f\n",
//                  setPoint, kp, ki, kd);
  }
}

// 讀取溫度，此函式將傳回浮點格式的攝氏溫度值
float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  uint16_t adc = analogRead(THERMO_PIN); // 讀取熱敏電阻感測值
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / (( 1 << ADC_BITS) - 1 - adc); // ADC 解析度為 10 位元

  // 轉換成攝氏溫度
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

float computePID(float in) {    // 計算 PID
  static float errorSum = 0;    // 累計誤差
  static float prevError = 0;   // 前次誤差值
  float error = setPoint - in;  // 當前誤差

  float dt = INTERVAL_MS / 1000.0;  // 間隔時間（秒）
  errorSum += error * dt;           // 積分：累計誤差
  errorSum = constrain(errorSum, 0, 255);     // 限制積分範圍
  float errorRate = (error - prevError) / dt; // 微分：誤差程度變化
  prevError = error; // 儲存本次誤差
  float output = kp * error + ki * errorSum + kd * errorRate; // 計算 PID
  output = constrain(output, 0, 255); // 限制輸出值範圍
  return output;
}

void setup() {
  Serial.begin(115200);
  pinMode(HEATER_PIN, OUTPUT);
  
  ledcSetup(PWM_CHANNEL, 1000, 8); // 設置 PWM
  ledcAttachPin(HEATER_PIN, PWM_CHANNEL);
  analogReadResolution(ADC_BITS); // 設定 ADC 解析度位元
}

void loop() {
  static uint32_t prevTime = 0; // 前次時間，宣告成「靜態」變數
  uint32_t now = millis();      // 目前時間
  
  if (now - prevTime >= INTERVAL_MS) {
    prevTime = now;
    
    float temp = readTemp(); // 讀取溫度
    float power = computePID(temp); // 計算 PID
    ledcWrite(PWM_CHANNEL, (int)power);

    Serial.printf("%.2f,%.2f\n", setPoint, temp);  // 輸出 PID 運算值
  }

  readSerial(); // 讀取序列輸入參數
}
