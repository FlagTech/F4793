/*
 程式更新以配合ESP32開發環境3.x版
 請參閱：https://swf.com.tw/?p=2000
*/

#define THERMO_PIN 36   // 熱敏電阻分壓輸入腳
#define HEATER_PIN 33   // 陶瓷加熱片的 PWM 輸出腳
#define ADC_BITS 10     // 類比輸入解析度 10 位元
#define INTERVAL_MS 1000  // 間隔時間 1 秒
#define PWM_CHANNEL 0     // PWM 通道

float setPoint = 40.0;  // 目標溫度
float kp = 0; // kp 參數，預設為 32，可透過序列埠更改

// 讀取溫度，此函式將傳回浮點格式的攝氏溫度值
float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  uint16_t adc = analogRead(THERMO_PIN); // 讀取熱敏電阻感測值
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / (( 1 << ADC_BITS) - 1 - adc); // ADC 解析度為 10 位元

  // 轉換成攝氏溫度
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

void readSerial() { // 讀取序列埠輸入的浮點 Kp 值
  while (Serial.available()) {
    kp = Serial.parseFloat(); // 讀取序列埠字串並解析成浮點數字
    // Serial.printf("kp= %.2f\n", kp); // 顯示用戶輸入的浮點數字
    // 遇到 '\n' 結尾時退出 while 迴圈
    if (Serial.read() == '\n') break;
  }
}

void setup() {
  pinMode(HEATER_PIN, OUTPUT);
  Serial.begin(115200);

  // 以下兩行適用於ESP32開發平台2.x版
  // ledcSetup(PWM_CHANNEL, 1000, 8); // 設置 PWM 通道、頻率和解析度
  // ledcAttachPin(HEATER_PIN, PWM_CHANNEL); // 設定 PWM 輸出腳
  // 底下敘述適用於ESP32開發平台3.x版
  ledcAttachChannel(HEATER_PIN, 1000, 8, PWM_CHANNEL); // 接腳, 頻率, 解析度, 通道
  analogReadResolution(ADC_BITS); // 設定 ADC 解析度位元
}

void loop() {
  static uint32_t prevTime = 0;   // 前次檢測時間
  uint32_t now = millis();        // 當前時刻

  if (now - prevTime >= INTERVAL_MS) {
    prevTime = now;
    float temp = readTemp(); // 讀取溫度
    float error = setPoint - temp; // 計算誤差
    float power = kp * error;
    
    // 計算 P 控制值
    power = constrain(power, 0, 255);   // 限制 PWM 的範圍
    // 底下敘述適用於ESP32開發平台2.x版
    // ledcWrite(PWM_CHANNEL, (int)power);  // 開始加熱
    // 底下敘述適用於ESP32開發平台3.x版
    ledcWrite(HEATER_PIN, (int)power); // 開始加熱

    // 向序列埠輸出溫度和 PWM 值
    // Serial.printf("%.2f,%d\n", temp, (int)power);
    Serial.printf("%.2f,%.2f\n", setPoint, temp);
  }
  
  readSerial(); // 讀取序列埠輸入值
}
