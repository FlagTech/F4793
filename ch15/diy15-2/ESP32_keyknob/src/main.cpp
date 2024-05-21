#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include <Arduino.h>
#include <USB.h>
#include <USBHIDConsumerControl.h>

#include "Knob.h"  // 引用「正交脈衝訊號編碼器」類別

Knob ko(4, 5, 6);          // 建立旋鈕物件（CLK, DT, SW）
USBHIDConsumerControl CC;  // 建立USB HID消費者控制物件

void setup() {
  ko.begin();   // 初始化旋鈕物件
  CC.begin();   // 初始化USB HID消費者控制物件
  USB.begin();  // 啟動USB
}

void loop() {
  switch (ko.dialChanged()) {  // 查詢旋鈕是否有變化
    case ko.DIR_CW:            // 若有順時針旋轉的訊號
      CC.press(CONSUMER_CONTROL_VOLUME_INCREMENT);  // 按下增大音量鍵
      CC.release();                                 // 釋放增大音量鍵
      break;
    case ko.DIR_CCW:  // 若有逆時針旋轉的訊號
      CC.press(CONSUMER_CONTROL_VOLUME_DECREMENT);  // 按下降低音量鍵
      CC.release();                                 // 釋放降低音量鍵
      break;
  }

  if (ko.pressed) {                         // 若有按鈕按下的訊號
    CC.press(CONSUMER_CONTROL_PLAY_PAUSE);  // 按下播放/暫停鍵
    ko.pressed = false;                     // 清除按鈕按下的訊號
  }
  if (ko.released) {      // 若有按鈕放開的訊號
    CC.release();         // 釋放播放/暫停鍵
    ko.released = false;  // 清除按鈕放開的訊號
  }
}
#endif