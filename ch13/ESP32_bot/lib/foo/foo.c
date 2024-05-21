#include "foo.h"

int counter = 5;  // 定義全域變數的值
void countUp() { counter++; }

int powerOut(int pwm) {
  pwm = constrain(pwm, -100, 100);
  return pwm;
}