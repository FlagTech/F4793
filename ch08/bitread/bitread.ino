void setup() {
  Serial.begin(115200);
  byte s0, s1, s2;

  for (byte n = 0; n < 8; n++) {
    s0 = bitRead(n, 0);  // 取得n的位置0的位元值
    s1 = bitRead(n, 1);  // 取得n的位置1的位元值
    s2 = bitRead(n, 2);  // 取得n的位置2的位元值

    Serial.printf("%d %d %d %d\n", n, s2, s1, s0);
  }
}

void loop () { }
