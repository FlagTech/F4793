/*
 程式更新以配合ESP32開發環境3.x版
 請參閱：https://swf.com.tw/?p=2000
*/

#define IN1 18
#define IN2 19
byte pwmChannel = 0;  // 指定PWM通道（全域變數）

void forward(int power) {  // 依指定power值「慢速衰減」正轉
  // ledcDetachPin(IN1); 
  ledcDetach(IN1); 
  digitalWrite(IN1, HIGH);          // IN1腳輸出高電位
  // ledcAttachPin(IN2, pwmChannel);
  // ledcWrite(pwmChannel, power);  // 調整PWM輸出值
  ledcAttachChannel(IN2, 1000, 10, pwmChannel); // 接腳, 頻率, 解析度, 通道
  ledcWrite(IN2, power);   // 調整PWM輸出值
}

void reverse(int power) {  // 依指定power值「慢速衰減」反轉
  // ledcDetachPin(IN2);
  ledcDetach(IN2);
  digitalWrite(IN2, HIGH);          // IN2腳輸出高電位
  // ledcAttachPin(IN1, pwmChannel);
  // ledcWrite(pwmChannel, power);  // 調整PWM輸出值
  ledcAttachChannel(IN1, 1000, 10, pwmChannel); // 接腳, 頻率, 解析度, 通道
  ledcWrite(IN1, power);   // 調整PWM輸出值
}

void stop() {  // 停止
  // ledcDetachPin(IN1);  // 解除IN1腳的PWM輸出
  // ledcDetachPin(IN2);  // 解除IN2腳的PWM輸出
  ledcDetach(IN1);  // 解除IN1腳的PWM輸出
  ledcDetach(IN2);  // 解除IN2腳的PWM輸出
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void brake() {  // 煞車
  // ledcDetachPin(IN1);  // 解除IN1腳的PWM輸出
  // ledcDetachPin(IN2);  // 解除IN2腳的PWM輸出
  ledcDetach(IN1);  // 解除IN1腳的PWM輸出
  ledcDetach(IN2);  // 解除IN2腳的PWM輸出
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); // 馬達接腳設成「輸出」
  pinMode(IN2, OUTPUT);
  // ledcSetup(pwmChannel, 1000, 10);  // 設置PWM：通道0, 1000Hz, 10位元
}

void loop() {
  Serial.println("正轉，PWM 1000");
  forward(1000);
  delay(3000);
  Serial.println("停止");
  stop();
  delay(1000);
  Serial.println("反轉，PWM 250");
  reverse(250);
  delay(3000);
  Serial.println("停止");
  stop();
  delay(1000);
}
