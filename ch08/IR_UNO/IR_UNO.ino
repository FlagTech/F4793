const byte IR_PINS[] = {A0, A1, A2, A3, A4, A5};  // 類比腳陣列

void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  for (byte i=0; i<6; i++) {
    val = analogRead(IR_PINS[i]);
    Serial.print(val);
    Serial.print(", ");
  }
  
  Serial.println(val);
  delay(100);
}
