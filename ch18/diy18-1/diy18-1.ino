#include <BluetoothSerial.h>  // 引用典型藍牙序列通訊程式庫
#include <esp32_can.h>
#include <esp32_obd2.h>
#define CRX_PIN  16   // CAN控制器的RX腳
#define CTX_PIN  17   // CAN控制器的TX腳
#define BT_DEVICE_NAME "ESP32 OBD2"  // 此開發板的藍牙名稱
#define LED_PIN 2     // 開發板內建的LED腳

BluetoothSerial SerialBT; // 宣告典型藍牙序列通訊物件

const int PIDS[] = {  // 儲存要測試的PID的常數名稱或編號的陣列
  CALCULATED_ENGINE_LOAD,  // 計算過的引擎負載
  ENGINE_COOLANT_TEMPERATURE,  // 引擎冷媒溫度
  ENGINE_RPM,  // 引擎每分鐘轉速
  VEHICLE_SPEED,  // 行車速度
  AIR_INTAKE_TEMPERATURE,  // 進氣溫度
  MAF_AIR_FLOW_RATE,   // 空氣流量感測器（MAF）空氣流率
  THROTTLE_POSITION,    // 油門位置
  RUN_TIME_SINCE_ENGINE_START,  // 引擎啟動後的運作時間
  FUEL_TANK_LEVEL_INPUT,  // 油箱液位輸入
  ABSOLULTE_BAROMETRIC_PRESSURE,  // 絕對大氣壓
  ABSOLUTE_LOAD_VALUE,  // 絕對負載值
  RELATIVE_THROTTLE_POSITION  // 相對油門位置
};

const int NUM_PIDS = sizeof(PIDS) / sizeof(PIDS[0]);  // 計算PIDS陣列的大小

void listPID() {  // 向藍牙序列埠輸出引擎轉速資料的函式
  for (int i = 0; i < NUM_PIDS; i++) {
    int pid = PIDS[i];  // 逐一取出要測試的PID的常數名稱或編號

    SerialBT.print(OBD2.pidName(pid));  // 讀取PID的名稱
    SerialBT.print(" = ");
    float pidValue = OBD2.pidRead(pid);  // 讀取PID的值

    if (isnan(pidValue)) {  // 若PID的值不是數字…
      SerialBT.print("資料值有誤！");
    } else {
      SerialBT.print(pidValue);  // 輸出PID的值
      SerialBT.print(" ");
      SerialBT.print(OBD2.pidUnits(pid));  // 輸出PID的單位
    }

    SerialBT.println();
  }

  SerialBT.println();
}

void RPM() {   // 向藍牙序列埠輸出引擎轉速資料的函式
  SerialBT.print("引擎轉速：");
  SerialBT.print(OBD2.pidRead(ENGINE_RPM));  // 取得引擎轉速值
  SerialBT.println(OBD2.pidUnits(ENGINE_RPM));  // 取得引擎轉速單位
}

void speed() {  // 向藍牙序列埠輸出行車速度的函式
  float spd = OBD2.pidRead(VEHICLE_SPEED);
  SerialBT.printf("行車速度：%.2fkm/h\n", spd);
}

void readBT() {  // 讀取藍牙序列埠輸入字串的函式
  if (SerialBT.available() > 0) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    if (input == "pid") {
      SerialBT.println("執行一些PID命令…");
      listPID();
    } else if (input == "RPM") {
      RPM();
    } else if (input == "speed") {
      speed();
    } 
  }
}

void setup() {
  SerialBT.begin(BT_DEVICE_NAME);  // 初始化藍牙序列埠
  pinMode(LED_PIN, OUTPUT);

  CAN0.setCANPins((gpio_num_t)CRX_PIN, (gpio_num_t)CTX_PIN);
  while (1) {
    SerialBT.println("嘗試連線到OBD2 CAN匯流排…");

    if (!OBD2.begin()) {
      SerialBT.println("無法連線！");
      digitalWrite(LED_PIN, HIGH);    // 閃爍LED
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    } else {
      SerialBT.println("連線成功！");
      digitalWrite(LED_PIN, HIGH);   // LED保持長亮
      break;
    }
  }
}

void loop() {
  readBT();
  delay(10);  // 不需要頻繁地檢查藍牙狀態，所以加入一點延遲時間。
}
