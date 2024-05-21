#define C1_PIN 33     // 霍爾輸出C1的接腳
#define C2_PIN 34     // 霍爾輸出C2的接腳

volatile int32_t count = 0;  // 將被中斷常式改變的值，要設成volatile。
int32_t lastCount = 0;       // 儲存「上次」脈衝數

void ARDUINO_ISR_ATTR encISR() {  // 計算脈衝數的中斷處理常式
  bool c2 = digitalRead(C2_PIN);  // 讀取C2的狀態
  if (c2 == LOW)  // 若c2為低電位…
    count++;  // 代表輪子那一面是順時針、那磁石面是逆時針轉動
  else
    count--;  // 代表輪子那一面是逆時針、磁石那面是順時針轉動
}

void setup() {
  Serial.begin(115200);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  // 偵測C1腳的變化，於脈衝上升階段觸發。
  attachInterrupt(C1_PIN, encISR, RISING);
}

void loop() {
  if (count != lastCount) {  // 若脈衝數變動，顯示脈衝數。
    lastCount = count;
    Serial.printf("脈衝數：%ld\n", count);
  }
}
