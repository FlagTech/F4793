#define C1_PIN 33     // 霍爾輸出C1的接腳
#define C2_PIN 34     // 霍爾輸出C2的接腳
#define BTN_PIN  27          // 歸零按鍵的接腳
#define DEBOUNCE_TIME 300   // 彈跳延遲時間

volatile uint32_t debounceTimer = 0;  // 儲存按鍵的彈跳時間
volatile int btnPressed = 0;   // 按鍵是否被按下

void ARDUINO_ISR_ATTR btnISR() {  // 按鍵中斷處理常式
  // 若按下時間等於或超過彈跳延遲時間…
  if ( (millis() - debounceTimer) >= DEBOUNCE_TIME) {
    btnPressed ++;  // 累計按鍵觸發次數
    debounceTimer = millis();  // 取得毫秒值
  }
}

volatile int32_t count = 0;  // 將被中斷常式改變的值，要設成volatile。
int32_t lastCount = 0;       // 儲存「上次」脈衝數

void ARDUINO_ISR_ATTR encISR() {  // 計算脈衝數的中斷處理常式
  bool c2 = digitalRead(C2_PIN);  // 讀取C2的狀態
  if (c2 == LOW)  // 若c2為低電位…
    count++;  // 代表輪子那面是順時針，磁石那一面是逆時針轉動。
  else
    count--;  // 代表輪子那面是逆時針，磁石那一面是順時針轉動。
}

void setup() {
  Serial.begin(115200);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  // 偵測C1腳的變化，於脈衝上升階段觸發。
  attachInterrupt(C1_PIN, encISR, RISING);

  pinMode(BTN_PIN, INPUT_PULLUP);  // 按鍵接腳，啟用上拉電阻。
  attachInterrupt(BTN_PIN, btnISR, RISING);  // 附加按鍵中斷常式
}

void loop() {
  if (btnPressed > 0) {  // 「啟動」鍵被按下了嗎？
    btnPressed = 0;      // 清除按鍵紀錄
    count = 0;          // 計數器清零
  }

  if (count != lastCount) {  // 若脈衝數變動，顯示脈衝數。
    lastCount = count;
    Serial.printf("脈衝數：%ld\n", count);
  }
}
