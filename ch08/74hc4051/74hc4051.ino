#define IR_PIN 36  // 類比輸入腳位（A0）
#define S0 32      // 切換通道編號的腳位
#define S1 33
#define S2 34

/* 參數n：指定類比通道的數字0~7。
   傳回值：指定通道的類比值。    */
uint16_t readIR(byte n) {
  digitalWrite(S0, bitRead(n, 0));
  digitalWrite(S1, bitRead(n, 1));
  digitalWrite(S2, bitRead(n, 2));
  return analogRead(IR_PIN);    // 傳回通道n的類比感測值
}

void setup() {
  Serial.begin(115200);
  pinMode (S0, OUTPUT);
  pinMode (S1, OUTPUT);
  pinMode (S2, OUTPUT);
  // 設定類比輸入埠的電壓上限（3.6V）和量化位元（10）
  analogSetAttenuation(ADC_11db);
  analogSetWidth(10);  // 設定類比取樣位元數
}

void loop () {
  for (byte i = 0; i < 8; i++) {
    Serial.printf("%d,", readIR(i));
  }
  Serial.println("");
  delay (100);
}
