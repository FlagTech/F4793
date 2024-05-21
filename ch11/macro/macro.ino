#include <Arduino.h>    // 必須引用這個標頭檔
#if __AVR_ATmega328P__
  #define LED_PIN 9    // 僅用於ATmega328P開發板
#elif CONFIG_IDF_TARGET_ESP32
  #define LED_PIN 2    // 僅用於ESP32開發板
  #define BITS 10
#endif

void setup() { 
  pinMode(LED_PIN, OUTPUT); 
  #if CONFIG_IDF_TARGET_ESP32
    analogReadResolution(BITS);  // 10位元寬度
    ledcSetup(0, 5000, BITS);    // 設定PWM，通道0、5KHz、10位元  
    ledcAttachPin(LED_PIN, 0);   // 指定內建的LED接腳成PWM輸出
  #endif
}

void loop() {
  int val = analogRead(A4);
  #if __AVR_ATmega328P__
    val = map(val, 0, 1024, 0, 255);
    analogWrite(LED_PIN, val);
  #elif CONFIG_IDF_TARGET_ESP32
    ledcWrite(0, val);
  #endif
}
