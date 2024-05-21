#include <BluetoothSerial.h>  // 藍牙序列埠

BluetoothSerial BT;           // 建立 ESP32 典型藍牙物件
float setpoint, kp, ki, kd; // 接收逗號分隔字串資料的變數

void readBTtask(void *pvParam) { // 讀取序列輸入字串的藍牙任務
  while (1) {
    if (BT.available()) { // 若藍牙收到資料…
      String msg = BT.readStringUntil('\n'); // 持續讀取，直到 '\n' 字元

      if (msg == "save") {
        BT.println("data saved.");
      } else {
        // 解析逗號分隔字串，把資料存入各個浮點型態變數
        sscanf(msg.c_str(), "%f,%f,%f,%f", &setpoint, &kp, &ki, &kd);
        // 在藍牙和有線序列埠輸出資料
        BT.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
                  setpoint, kp, ki, kd);
        Serial.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
                      setpoint, kp, ki, kd);
      }
    }

    vTaskDelay(1); // 延遲 1ms
  }
}

void writeBTtask(void *pvParam) {
  while (1) {
    BT.println("hello");
    vTaskDelay(2000); // 延遲 2000 毫秒
  }
}

void setup() {
  Serial.begin(115200);
  BT.begin("ESP32溫控板"); // 啟用典型藍牙

  // 在核心 0 建立兩個任務
  xTaskCreate( readBTtask, "BT read", 2048, NULL, 0 , NULL);
  xTaskCreate( writeBTtask, "BT write", 1024, NULL, 0 , NULL);
}

void loop() { }
