void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');  // 持續讀取到 '\n' 結尾
    int index = str.indexOf(',');               // 取得字串中的 ',' 位置
    
    if (index != -1) {  // 如果有找到 ',' 字元… 
      String n1 = str.substring(0, index);    // 取得 ',' 字元之前的子字串
      String n2 = str.substring(index + 1);   // 取得 ',' 字元之後的子字串
      Serial.printf("數字1：%d\n", n1.toInt()); // 把字串轉成整數
      Serial.printf("數字2：%d\n", n2.toInt());
    }
  }
}
