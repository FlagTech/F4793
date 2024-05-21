#define IR_LATCH_PIN 16   // PE並列開關接腳
#define IR_CLOCK_PIN 17   // CP時脈接腳
#define IR_DATA_PIN  36  // Q7序列資料輸出接腳

uint8_t readIR() {
  uint8_t IR_data = 0;   // 儲存輸入資料值
  // 操作PE和CP腳，把並列資料讀入74HC166。
  digitalWrite(IR_LATCH_PIN, LOW);  // PE腳設成低電位
  digitalWrite(IR_CLOCK_PIN, LOW);  // CP時脈腳設成低電位
  digitalWrite(IR_CLOCK_PIN, HIGH);  // CP時脈腳設成高電位
  digitalWrite(IR_LATCH_PIN, HIGH);  // PE腳設成低電位

  // CP時脈腳設成低電位，準備讀入資料。
  digitalWrite(IR_CLOCK_PIN, LOW);
  // 透過shiftIn()函式，從最高位元依序把資料存入IR_data變數
  IR_data = shiftIn(IR_DATA_PIN, IR_CLOCK_PIN, MSBFIRST);
  return IR_data;
}

void setup() {
  Serial.begin(115200);
  pinMode(IR_LATCH_PIN, OUTPUT);
  pinMode(IR_CLOCK_PIN, OUTPUT);
  pinMode(IR_DATA_PIN, INPUT);
}

void loop() {
  byte IR = readIR();     // 讀取循跡模組資料
  Serial.println(IR, BIN);  // 以2進位方式顯示感測資料
  delay(100);
}
