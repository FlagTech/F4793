#include <Arduino.h>

#include "avr8-stub.h"

const int8_t LED_PIN = 13;

void blink(int times, int interval) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(interval);
    digitalWrite(LED_PIN, LOW);
    delay(interval);
  }
}

void setup() {
  debug_init();  // 初始化偵錯器
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  static uint8_t count = 0;
  breakpoint();  // 設定中斷點

  if (count < 3) {
    blink(3, 100);
  } else {
    blink(3, 500);
    count = 0;
  }

  count++;
}