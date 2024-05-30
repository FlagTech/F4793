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
float errorSum = 0;     // 累計誤差

void readSerial() {
  if (Serial.available()) { // 若有序列資料輸入…
    String txt = Serial.readStringUntil('\n');  // …持續讀到 '\n' 並存入 txt
    // 解析逗號分隔字串，分成 3 個浮點資料分別存入 setpoint,kp 和 ki
    sscanf(txt.c_str(), "%f,%f,%f", &setPoint, &kp, &ki);
    // 顯示剛剛讀入的參數資料值
    //Serial.printf("setPoint= %f, kp= %f, ki= %f\n", setPoint, kp, ki);
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

float computePI(float in) {
  float error = setPoint - in;      // 計算誤差
  float dt = INTERVAL_MS / 1000.0;  // 間隔時間（單位是秒）

  errorSum += error * dt;// 累計誤差
  errorSum = constrain(errorSum, 0, 255);     // 限制累計誤差值的大小
  float output = kp * error + ki * errorSum;  // 計算 PI
  output = constrain(output, 0, 255);         // 限制輸出值的大小
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
  static uint32_t prevTime = 0;   // 前次檢測時間
  uint32_t now = millis();

  if (now - prevTime >= INTERVAL_MS) {
    prevTime = now;

    float temp = readTemp();              // 讀取溫度
    float power = (int) computePI(temp);  // 計算 PI

    ledcWrite(PWM_CHANNEL, (int)power);   // 加熱
    Serial.printf("%.2f,%.2f\n", setPoint, temp);
  }

  readSerial(); // 讀取序列埠輸入值
}
