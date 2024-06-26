#define IN1 18
#define IN2 19
#define PWM_BIT_RESOLUTION 10  // PWM位元解析度
byte pwmChannel = 0;  // 指定PWM通道（全域變數）

void forward(int power) {  // 依指定PWM值「慢速衰減」正轉
  ledcDetachPin(IN1);  // 解除IN1腳的PWM輸出
  digitalWrite(IN1, HIGH);  // IN1腳輸出高電位
  ledcAttachPin(IN2, pwmChannel);  // IN2腳附加PWM輸出

  power = ((1 << PWM_BIT_RESOLUTION) - 1) - power;
  ledcWrite(pwmChannel, power);  // 調整PWM輸出值
}

void reverse(int power) {  // 依指定PWM值「慢速衰減」反轉
  ledcDetachPin(IN2);   // 解除IN2腳的PWM輸出
  ledcAttachPin(IN1, pwmChannel);  // IN1腳附加PWM輸出
  digitalWrite(IN2, HIGH);      // IN2腳輸出高電位

  power = ((1 << PWM_BIT_RESOLUTION) - 1) - power;
  ledcWrite(pwmChannel, power);  // 調整PWM輸出值
}

void stop() {  // 停止
  ledcDetachPin(IN1);  // 解除IN1腳的PWM輸出
  ledcDetachPin(IN2);  // 解除IN2腳的PWM輸出
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void brake() {  // 煞車
  ledcDetachPin(IN1);  // 解除IN1腳的PWM輸出
  ledcDetachPin(IN2);  // 解除IN2腳的PWM輸出
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); // 馬達接腳設成「輸出」
  pinMode(IN2, OUTPUT);
  ledcSetup(pwmChannel, 1000, 10);  // 設置PWM：通道0, 1000Hz, 10位元
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
