#define XOR_PIN 33     // 霍爾XOR輸出
#define C2_PIN 34      // 霍爾輸出C2

volatile bool lastC1;   // 儲存「上次」霍爾C1值
volatile bool lastC2;   // 儲存「上次」霍爾C1值
volatile int16_t count = 0; // 儲存脈衝數
int16_t lastCount = 0;  // 儲存「上次」脈衝數

void ARDUINO_ISR_ATTR encISR() {  // 中斷處理函式
  bool newC2 = digitalRead(C2_PIN);
  bool newC1 = digitalRead(XOR_PIN) ^ newC2;

  count += (newC1 ^ lastC2) - (lastC1 ^ newC2);  // 計算馬達轉動次數
  
  lastC1 = newC1;  // 儲存本次的C1和C2狀態
  lastC2 = newC2;
}

void setup() {
  Serial.begin(115200);
  pinMode(XOR_PIN, INPUT);  // 數位腳設為輸入模式
  pinMode(C2_PIN, INPUT);
  attachInterrupt(XOR_PIN, encISR, CHANGE);  // 附加中斷處理常式
  
  lastC2 = digitalRead(C2_PIN);  // 讀取當前霍爾C2和C1的狀態
  lastC1 = digitalRead(XOR_PIN) ^ lastC2;
}

void loop() {
  if (count != lastCount) {  // 若脈衝數有變動，則輸出脈衝數。
    lastCount = count;
    Serial.printf("脈衝數：%d\n", count);
  }
}
