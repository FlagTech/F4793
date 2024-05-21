#include <Keypad.h>
#define COLS 3   // 3行按鍵
#define ROWS 3   // 3列按鍵

byte colPins[COLS] = {18, 37, 39};  // 按鍵模組，行1~3接腳。
byte rowPins[ROWS] = {16, 3, 5};  // 按鍵模組，列1~3接腳。

char keys[ROWS][COLS] = {  // 定義每個按鍵的代表字元
  {'0', '1', '2'},
  {'3', '4', '5'},
  {'6', '7', '8'}
};

// 定義鍵盤物件
Keypad kp = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(115200);
}

void loop() {
  String msg = "";  // 訊息字串

  if (kp.getKeys()) {  // 若有任何鍵被按下…
    for (uint8_t i = 0; i < LIST_MAX; i++) { // 掃描key陣列的每個按鍵元素
      if ( kp.key[i].stateChanged ) { // 若有按鍵的狀態發生變化…
        switch (kp.key[i].kstate) {  // 紀錄按鍵的狀態
          case PRESSED:
            msg = "按下";
            break;
          case HOLD:
            msg = "按著";
            break;
          case RELEASED:
            msg = "放開";
            break;
          case IDLE:
            msg = "放空";
            break;
        }

        Serial.printf("%s %c\n", msg, kp.key[i].kchar);  // 顯示按鍵的狀態和名稱
      }
    }
  }
}