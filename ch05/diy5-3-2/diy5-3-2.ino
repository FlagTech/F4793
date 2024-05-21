#define C1_PIN 33     // 霍爾輸出C1
#define C2_PIN 34     // 霍爾輸出C2
#define WHEEL_DIAM  34  // 輪胎直徑（單位:mm）
#define PPR 28  // 每圈脈衝數（Pulse Per Rotation）
#define GEAR_RATIO 30  // 齒輪比
#define CIRCUM WHEEL_DIAM * PI  // 輪胎周長
#define WHEEL_RATIO CIRCUM / (PPR * GEAR_RATIO)
#define BTN_PIN  27  // 歸零鍵

volatile int32_t count = 0; // 儲存脈衝數
int32_t lastCount = 0;  // 儲存「上次」脈衝數
volatile uint32_t prevPressTime;   // 儲存按鍵被按下的時間

float distance = 0;  // 移動距離

void ARDUINO_ISR_ATTR ISR_C1() {    // 霍爾中斷處理常式
  if (digitalRead(C1_PIN) == HIGH) {   // 若C1腳是高電位…
    if (digitalRead(C2_PIN) == LOW) {  // 若C2腳是低電位…
      count++;
    } else {
      count--;
    }
  } else {    // C1腳是低電位…
    if (digitalRead(C2_PIN) == LOW) {
      count--;
    } else {
      count++;
    }
  }
}

void ARDUINO_ISR_ATTR ISR_C2() {    // 霍爾C2腳中斷處理常式
  if (digitalRead(C2_PIN) == HIGH) {   // 若C2腳是高電位…
    if (digitalRead(C1_PIN) == HIGH) {  // 若C1腳是高電位…
      count++;   // 代表輪子那面是順時針，磁石那一面是逆時針轉動。
    } else {
      count--;
    }
  } else {    // C2腳是低電位…
    if (digitalRead(C1_PIN) == HIGH) { // 若C1腳是高電位…
      count--;  // 逆時針轉動，減少計數值。
    } else {
      count++;  // 順時針轉動，增加計數值。
    }
  }
}

void ARDUINO_ISR_ATTR ISR_BTN() {
  if (millis() - prevPressTime > 100) {
    count = 0;
    prevPressTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(C1_PIN, INPUT);  // 霍爾C1
  pinMode(C2_PIN, INPUT);  // 霍爾C2
  pinMode(BTN_PIN, INPUT_PULLUP);  // 按鍵接腳，上拉電阻。

  // 附加霍爾編碼器的中斷處理常式，於訊號改變時觸發。
  attachInterrupt(C1_PIN, ISR_C1, CHANGE);  // 在C1腳附加訊號變化中斷
  attachInterrupt(C2_PIN, ISR_C2, CHANGE);  // 在C2腳附加訊號變化中斷
  attachInterrupt(BTN_PIN, ISR_BTN, RISING);  // 附加「啟動」鍵中斷常式

  Serial.println("開始測量距離!");
  prevPressTime = millis();
}

void loop() {
  // 若脈衝數有變動，則輸出脈衝數。
  if (lastCount != count) {
    lastCount = count;    // 儲存此次編碼值
    // 移動距離 = 脈衝數 × 周長 ÷ (每圈脈衝數 × 減速比) 
    distance = count * WHEEL_RATIO;
    Serial.printf("移動距離：%.2fmm\n", distance);
  }
}
