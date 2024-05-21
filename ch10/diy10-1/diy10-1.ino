#define IR_LATCH_PIN 16
#define IR_CLOCK_PIN 17
#define IR_DATA_PIN  36

uint16_t checkLine() {
  uint8_t IR_data[8];   // 紀錄黑線位置
  uint8_t cnt = 0;      // 紀錄黑線寬
  float   middle;        // 中間值
  uint16_t lineRaw = 0; // 感測位置

  digitalWrite(IR_LATCH_PIN, LOW);
  digitalWrite(IR_CLOCK_PIN, LOW);
  digitalWrite(IR_CLOCK_PIN, HIGH);
  digitalWrite(IR_LATCH_PIN, HIGH);

  for (int8_t i = 7; i >= 0 ; i--) {
    digitalWrite(IR_CLOCK_PIN, LOW);
    uint8_t val = digitalRead(IR_DATA_PIN);
    if (val) {
      IR_data[cnt] = i + 1; // 索引範圍：8~1
      cnt ++;
    }
    digitalWrite(IR_CLOCK_PIN, HIGH);
  }

  if (cnt == 0) return 0;  // 若沒有感測到黑線…

  if (cnt == 1) {  // 只感應到一點
    middle = IR_data[0];  // 取得索引對應的權值
  } else if (cnt % 2 == 0) { // 偶數，取中間兩數的平均值。
    uint8_t index = cnt / 2;
    middle = (IR_data[index] + IR_data[index - 1]) / 2.0;
  } else {   // 奇數，直接取中間值。
    uint8_t index = cnt / 2;
    middle = IR_data[index];
  }
  lineRaw = middle * 1000 / 8; // 取得索引對應的權值
 
  return lineRaw;
}


void setup() {
  Serial.begin(115200);
  pinMode(IR_LATCH_PIN, OUTPUT);
  pinMode(IR_CLOCK_PIN, OUTPUT);
  pinMode(IR_DATA_PIN,  INPUT);
}

void loop() {
  uint16_t pos = checkLine();  // 取得車體位置
  Serial.println(pos);
  delay(100);
}
