#include <USB.h>
#include <USBHIDConsumerControl.h>  // 多媒體控制程式庫
#include <USBHIDKeyboard.h>         // 標準鍵盤程式庫
#include <USBHIDMouse.h>            // 滑鼠程式庫
#include <USBHIDSystemControl.h>    // 系統控制程式庫
#include <Keypad.h> // 引用Keypad程式庫
#define ROWS 4  // 按鍵模組的列數
#define COLS 4  // 按鍵模組的行數

USBHIDConsumerControl ConsumerControl;  // 「多媒體控制」物件
USBHIDSystemControl SystemControl;      // 「系統控制」物件
USBHIDKeyboard Keyboard;                // 「標準鍵盤」物件
USBHIDMouse Mouse;                      // 「滑鼠」物件

// 依照行、列排列的按鍵字元（二維陣列）
char keymap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[COLS] = {7, 6, 5, 4};      // 按鍵模組，行1~4接腳。
byte rowPins[ROWS] = {18, 17, 16, 15};  // 按鍵模組，列1~4接腳。

Keypad kp = Keypad(makeKeymap(keymap),
                         rowPins, colPins, ROWS, COLS);

void setup() {
  ConsumerControl.begin(); // 初始化多媒體控制物件
  Keyboard.begin();        // 初始化普通鍵盤物件
  SystemControl.begin();   // 初始化系統控制物件
  USB.begin();             // 初始化USB物件
}

void loop() {
  char key = kp.getKey();   // 讀取按鍵的字元

  switch (key) { // 若有按鍵被按下…
    case '*':  // 降低音量
      ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
      ConsumerControl.release();  // 送出「放開按鍵」訊號
      break;
    case '#':  // 調高音量
      ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
      ConsumerControl.release();  // 送出「放開按鍵」訊號
      break;
    case '5':
      Keyboard.print("love DIY!"); // 送出"love DIY!"字串
      Keyboard.write(KEY_RETURN);  // 送出"return"字元
      break;
    case '4':  // 睡眠
      SystemControl.press(SYSTEM_CONTROL_STANDBY);
      SystemControl.release();     // 送出「放開按鍵」訊號
      break;
    case '7':  // 滑鼠左鍵
      Mouse.click(MOUSE_LEFT);
      break;
    case '2':  // 滑鼠往上移動1像素
      Mouse.move(0, -1);
      break;
    case '0':  // 滑鼠往下移動1像素
      Mouse.move(0, 1);
      break;
    case 'C':  // 啟動Copliot助理（AI鍵）
      Keyboard.press(KEY_LEFT_GUI); // 按著左Windows鍵
      Keyboard.press('c');          // 按下'C'字鍵
      Keyboard.releaseAll();        // 送出「放開按鍵」訊號
      break;
    case 'A':  // 截圖
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(0xCE);   // KEY_PRINT_SCREEN的鍵碼
      Keyboard.releaseAll();  // 送出「放開按鍵」訊號
      break;
  }
}
