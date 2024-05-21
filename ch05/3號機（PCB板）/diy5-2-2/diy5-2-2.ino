#define C1_PIN 33     // 霍爾輸出C1的接腳
#define C2_PIN 34     // 霍爾輸出C2的接腳
#define BTN_PIN 4            // 歸零按鍵的接腳

volatile int32_t count = 0;  // 將被中斷常式改變的值，要設成volatile。
int32_t lastCount = 0;       // 儲存「上次」脈衝數

void ARDUINO_ISR_ATTR encISR() {    // 霍爾中斷處理常式
  if (digitalRead(C1_PIN) == HIGH) {   // 若C1腳是高電位…
    if (digitalRead(C2_PIN) == LOW) {  // 若C2腳是低電位…
      count++;   // 代表輪子那面是順時針，磁石那一面是逆時針轉動。
    } else {
      count--;   // 代表輪子那面是逆時針，磁石那一面是順時針轉動。
    }
  } else {    // C1腳是低電位…
    if (digitalRead(C2_PIN) == LOW) {
      count--;
    } else {
      count++;
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(C1_PIN, INPUT);  // 霍爾C1
  pinMode(C2_PIN, INPUT);  // 霍爾C2
  pinMode(BTN_PIN, INPUT);  // 按鍵接腳

  // 附加霍爾編碼器的中斷處理常式，於訊號改變時觸發。
  attachInterrupt(C1_PIN, encISR, CHANGE);
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

  if (start) {   // 若開關被按了一下
    count = 0;   // 計數器清零
    start = !start;
  }

  if (lastCount != count) {  // 若編碼值改變了…
    lastCount = count;    // 儲存此次編碼值
    Serial.printf("脈衝數：%d\n", count);
  }

  lastBtnState = btnState;  // 更新按鍵狀態
}
