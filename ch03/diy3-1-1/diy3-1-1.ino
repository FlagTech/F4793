#include <BluetoothSerial.h>  // 引用典型藍牙序列埠程式庫

BluetoothSerial BT;           // 建立 ESP32 典型藍牙物件
float setpoint, kp, ki, kd;   // 接收逗號分隔字串資料的變數

void setup() {
  Serial.begin(115200); // 用於顯示調校 PID 的訊息
  if (!BT.begin("ESP32溫控板")) { // 啟用典型藍牙
    Serial.println("無法初始化藍牙，重新啟動ESP32…");
    ESP.restart(); // 重新啟動 ESP32
  } else {
    Serial.println("藍牙初始化完畢！");
  }
}

void loop() {
  if (BT.available()) { // 若藍牙收到資料…
    String msg = BT.readStringUntil('\n');  // 持續讀取，直到 '\n' 字元
    if (msg == "save") {  // 若輸入資料是 "save"
      BT.println("data saved."); // 傳回 "data saved."（資料已存）
      return; // 結束本次資料處理
    }
    // 解析逗號分隔字串，把資料存入各個浮點型態變數
    sscanf(msg.c_str(), "%f,%f,%f,%f", &setpoint, &kp, &ki, &kd);
    // 在藍牙和有線 UART 序列埠輸出資料
    BT.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
              setpoint, kp, ki, kd);
    Serial.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
                  setpoint, kp, ki, kd);
  }
}
