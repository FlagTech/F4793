#include <Keypad.h>    // 引用Keypad程式庫
#define ROWS 4 // 按鍵模組的列數
#define COLS 4 // 按鍵模組的行數

const String password = "1212";   // 密碼
String key_in = "";              // 儲存用戶輸入的密碼

// 依照行、列排列的按鍵字元（二維陣列）
char keymap[ROWS][COLS] = {
  {'1', '2', '3', 'H'},
  {'4', '5', '6', 'E'},
  {'7', '8', '9', 'L'},
  {'*', '0', '#', 'O'}
};

byte colPins[COLS] = {7, 6, 5, 4};      // 按鍵模組，行1~4接腳。
byte rowPins[ROWS] = {18, 17, 16, 15};  // 按鍵模組，列1~4接腳。

// 定義並初始化Keypad物件
Keypad kp = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);

void setup(){
  Serial.begin(115200);
}

void loop(){
  char key = kp.getKey();  // 取得被按下按鍵的對應字元

  if (key){   // 如果有按鍵被按下
    Serial.println(key);  // 顯示按鍵的對應字元

    if(key == '*') {   // 清除輸入
      key_in = "";  // 清除輸入的字串
      Serial.println("清除輸入");
    } else if(key == '#') {  // 確認輸入
      if(password == key_in) {
        Serial.println("通關！");
      } else {
        Serial.println("密碼錯誤，請重新輸入。");
      }

      key_in = ""; // 清除輸入的字串
    } else {
      key_in += key; // 串接用戶輸入的字元
    }
  }
}
