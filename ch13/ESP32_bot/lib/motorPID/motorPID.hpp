#include <Arduino.h>

class PID {
 private:
  float kp, ki, kd;      // PID參數
  double errorSum = 0;   // 累計誤差
  double prevError = 0;  // 前次誤差值
  uint16_t sampleTime;
  uint16_t setpoint;

 public:
  PID(uint16_t target, float p = 1, float i = 0, float d = 0,
      uint16_t t = 1000) {
    setpoint = target;
    kp = p;
    ki = i;
    kd = d;
    sampleTime = t;
  }
  // 設定PID參數
  void setParams(uint16_t target, float p = 1, float i = 0, float d = 0) {
    setpoint = target;
    kp = p;
    ki = i;
    kd = d;
  }

  // 計算PID值的方法
  // input參數：編碼器的輸入值
  float compute(int16_t input) {
    double dT = (double)sampleTime / 1000;  // 取樣間隔時間
    int16_t error = setpoint - input;

    errorSum += error * dT;  // 累計誤差
    errorSum = constrain(errorSum, 0, 60);
    // 微分：誤差程度變化
    double errorRate = kd * ((error - prevError) / dT);
    prevError = error;  // 儲存本次誤差
    // 計算PID
    float output = kp * error + ki * errorSum + errorRate;
    output = constrain(output, 0, 100);

    return output;
  }
};
