#include <USB.h>
#include <USBHIDGamepad.h>
#define LEFT_X_PIN  9   // 左類比搖桿VRx腳
#define LEFT_Y_PIN  1   // 左類比搖桿VRy腳
#define LEFT_SW_PIN  3  // 左類比搖桿SW腳
#define A_PIN 39        // 微觸開關A
#define B_PIN 40        // 微觸開關B
#define COMBO_PIN 41    // 微觸開關B
#define BUTTONS_TOTAL 15  // 一共15個按鍵

uint16_t btnState = 0;    // 暫存按鍵狀態
uint16_t btnStateTemp = 0;

USBHIDGamepad gp;  // 宣告遊戲控制器物件

// 宣告15個按鍵接腳的陣列，按鍵必須依此順序排列。
// 本範例只連接3個按鍵，未使用的按鍵腳位設成-1。
int8_t buttons[BUTTONS_TOTAL] = {
  A_PIN,  // A
  B_PIN,  // B
  -1,     // C（自定義）
  -1,     // X
  -1,     // Y
  -1,     // Z
  -1,     // 左肩
  -1,     // 右肩
  -1,     // 左板機
  -1,     // 右板機
  -1,     // 選擇
  -1,     // 啟動
  -1,     // 模式
  LEFT_SW_PIN,  // 左拇指
  -1      // 右拇指
};

bool combo() {
  static uint8_t i = 0;  // 十字鍵的索引
  uint8_t hat[] = {HAT_RIGHT, HAT_DOWN, HAT_DOWN_RIGHT}; // 十字鍵
  uint8_t total = sizeof(hat);  // 十字鍵數

  if (i < total) {
    gp.send(0, 0, 0, 0, 0, 0, hat[i], 0);  // 送出十字鍵的狀態
    i++;
  } else {
    i = 0;
    gp.send(0, 0, 0, 0, 0, 0, 0, 0x1);  // 送出A鍵被按下
    delay(500);   // 延遲0.5秒，方便觀察狀態。
    return true;  // 組合鍵完成
  }

  delay(500);    // 延遲0.5秒，方便觀察狀態。
  return false;  // 組合鍵尚未完成
}

// 讀取全部搖桿和按鍵的狀態
void scanBtn() {
  // 讀取左搖桿值，原始資料範圍是0~1023，要轉換成-127~128。
  int8_t x = analogRead(LEFT_X_PIN) / 4 - 127;  // 水平（X）搖桿的輸入值
  int8_t y = analogRead(LEFT_Y_PIN) / 4 - 127;  // 垂直（Y）搖桿的輸入值

  // 暫存全部按鍵的狀態，參閱下文說明。
  for (uint8_t i = 0; i < BUTTONS_TOTAL; i++) {
    if (buttons[i] != -1) {
       btnStateTemp |= (!digitalRead(buttons[i])) << i;  // 讀取所有按鍵
    }
  }
  
  // 假如按鍵的狀態跟上次不同…
  if (btnStateTemp != btnState) {
    btnState = btnStateTemp;  // 儲存本次狀態
  }

  btnStateTemp = 0;  // 按鍵狀態清零
  gp.send(x, y, 0, 0, 0, 0, 0, btnState);  // 送出遊戲控制器的狀態
}

void setup() {
  // 把全部有接線的按鍵接腳都設為「啟用上拉電阻的輸入模式」
  for (uint8_t i = 0; i < BUTTONS_TOTAL; i++) {
    if (buttons[i] != -1) {
      pinMode(buttons[i], INPUT_PULLUP);
    }
  }

  // 組合鍵
  pinMode(COMBO_PIN, INPUT_PULLUP);

  analogSetAttenuation(ADC_11db);  // 設定類比輸入電壓上限3.6V
  analogReadResolution(10);      // 設定類比輸入解析度

  gp.begin();   // 啟用遊戲控制器
  USB.begin();  // 啟用USB介面
}

void loop() {
  static bool comboDone = true;
  scanBtn();   // 掃描按鍵

  bool comboBtn = !digitalRead(COMBO_PIN);
  if (comboBtn || !comboDone) {
    // 執行組合鍵
    comboDone = combo();
  }
  delay(5);
}
