#define C1_PIN 33     // 霍爾輸出C1的接腳
#define C2_PIN 34     // 霍爾輸出C2的接腳
#define BTN_PIN 27          // 歸零按鍵的接腳
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

void ARDUINO_ISR_ATTR encISR() {    // 霍爾中斷處理常式
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


void setup() {
  Serial.begin(115200);
  pinMode(C1_PIN, INPUT);  // 霍爾C1
  pinMode(C2_PIN, INPUT);  // 霍爾C2
  pinMode(BTN_PIN, INPUT_PULLUP);  // 按鍵接腳，上拉電阻。

  // 附加霍爾編碼器的中斷處理常式，於訊號改變時觸發。
  attachInterrupt(C1_PIN, encISR, CHANGE);
  attachInterrupt(BTN_PIN, btnISR, RISING);  // 附加「啟動」鍵中斷常式
}

void loop() {
  if (btnPressed > 0) {  // 「啟動」鍵被按下了嗎？
    btnPressed = 0;      // 清除按鍵紀錄
    count = 0;          // 計數器清零
  }

  if (lastCount != count) {  // 若編碼值改變了…
    lastCount = count;    // 儲存此次編碼值
    Serial.printf("脈衝數：%d\n", count);
  }
}
