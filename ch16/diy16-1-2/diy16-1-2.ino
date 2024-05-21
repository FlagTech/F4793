#include <USB.h>
#include <USBHIDKeyboard.h>         // 標準鍵盤程式庫
#include <Keypad.h>
#define COLS 3   // 3行
#define ROWS 3   // 3列

byte colPins[COLS] = {18, 37, 39};  // 按鍵模組，行1~3接腳。
byte rowPins[ROWS] = {16, 3, 5};  // 按鍵模組，列1~3接腳。

char keys[ROWS][COLS] = {    // 定義按鍵的鍵碼
  {'z', KEY_UP_ARROW, 'v'},
  {KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW},
  {KEY_LEFT_CTRL, 'c', KEY_RIGHT_GUI}
};

// 定義鍵盤物件
Keypad kp = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
USBHIDKeyboard Keyboard;   // 「標準鍵盤」物件

void setup() {
  Keyboard.begin();        // 初始化普通鍵盤物件
  USB.begin();             // 初始化USB物件
}

void loop() {
  if (kp.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) { // 掃描每個按鍵
      if ( kp.key[i].stateChanged ) { // 若有按鍵的狀態發生變化…
        switch (kp.key[i].kstate) {  // 檢查該按鍵的狀態
          case PRESSED:
            Keyboard.press(kp.key[i].kchar);  // 按下此按鍵
            break;
          case RELEASED:
            Keyboard.release(kp.key[i].kchar); // 放開此按鍵
            break;
        }
      }
    }
  }
}
