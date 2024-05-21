#include <BluetoothSerial.h>  // 藍牙序列埠
BluetoothSerial BT;           // 建立 ESP32 典型藍牙物件
float setpoint, kp, ki, kd; // 接收逗號分隔字串資料的變數

struct bt_data {      // 自訂儲存藍牙資料的結構體
  char txt[50] = "\0";  // 儲存序列輸入字串的成員
  bool updated = false; // 代表資料是否已更新，預設「未更新」
} btData;             // 宣告儲存藍牙輸入資料的變數


void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  switch (event) {
    case ESP_SPP_OPEN_EVT:      // 藍牙從端已連線
    case ESP_SPP_SRV_OPEN_EVT:  // 藍牙主控端已連線
      Serial.println("藍牙已連線。");
      break;
    case ESP_SPP_CLOSE_EVT:     // 「藍牙斷線
      Serial.println("藍牙斷線了！");
      break;
    case ESP_SPP_DATA_IND_EVT:    // 收到資料
      if (BT.available()) {       // 確認可取用資料
        String msg = BT.readStringUntil('\n'); // 讀取到 '\n'為止
        if (msg == "save") {
          BT.println("data saved."); return;
        }
        sscanf(msg.c_str(), "%f,%f,%f,%f", &setpoint, &kp, &ki, &kd);
        BT.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
                  setpoint, kp, ki, kd);
        Serial.printf("setpoint= %f, kp= %f, ki= %f, kd= %f\n",
                      setpoint, kp, ki, kd);
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);

  if (!BT.begin("ESP32溫控板")) {    // 啟用典型藍牙
    Serial.println("無法初始化藍牙通訊，重新啟動ESP32…");
    ESP.restart();
  } else {
    Serial.println("藍牙初始化完畢！");
  } BT.register_callback(btCallback); // 註冊回呼函式
}

void loop() { }
