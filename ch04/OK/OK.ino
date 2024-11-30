/*
 程式更新以配合ESP32開發環境3.x版
 請參閱：https://swf.com.tw/?p=2000
*/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <U8g2lib.h>
#define THERMO 36       // 熱敏電阻分壓輸入腳
#define HEATER 33       // 陶瓷加熱片的PWM輸出腳
#define INTERVAL 500    // 0.5秒
#define PWM_CHANNEL 0   // PWM通道（0~15）
#define PWM_BITS 8      // 8位元解析度
#define PWM_FREQ 1000   // PWM頻率1KHz
#define ADC_BITS 10     // 類比輸入ADC的解析度

const char *ssid = "你的Wi-Fi名稱";
const char *password = "你的Wi-Fi密碼";
String ipAddr = "";  // 儲存IP位址

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

float setPoint = 40.0;  // 目標溫度
float temp = 0;         // 目前溫度
float kp = 20.5;         // 比例增益
float ki = 30.25;         // 積分增益
float kd = 50.0;         // 微分增益

AsyncWebServer server(80); // 建立HTTP伺服器物件
AsyncWebSocket ws("/ws");  // 建立WebSocket物件

void onSocketEvent(AsyncWebSocket *server,
                   AsyncWebSocketClient *client,
                   AwsEventType type,
                   void *arg,
                   uint8_t *data,
                   size_t len)
{
  // 底下是ArduinoJson V6的JSON文件變數宣告
  // StaticJsonDocument<50> doc;
  // 底下是ArduinoJson V7的JSON文件變數宣告
  JsonDocument doc;
  
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("來自%s的用戶%u已連線\n", client->remoteIP().toString().c_str(), client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("用戶%u已離線\n", client->id());
      break;
    case WS_EVT_ERROR:
      Serial.printf("用戶%u出錯了：%s\n", client->id(), (char *)data);
      break;
    case WS_EVT_DATA:
      Serial.printf("用戶%u傳入資料：%s\n", client->id(), (char *)data);
      DeserializationError error = deserializeJson(doc, data, len);

      if (error) {
        Serial.print("解析JSON出錯了：");
        Serial.println(error.c_str());
        return;
      }
      /*
         傳入的JSON格式範例：
         {"PID":"p", "v":4.5}
      */
      const char *pidStr = doc["PID"];
      float val = doc["v"];   // 資料值
      if (strcmp(pidStr, "p") == 0) {
        Serial.printf("Kp: %.2f\n", val);
        kp = val;
      } else if (strcmp(pidStr, "i") == 0) {
        Serial.printf("Ki: %.2f\n", val);
        ki = val;
      } else if (strcmp(pidStr, "d") == 0) {
        Serial.printf("Kd: %.2f\n", val);
        kd = val;
      } else if (strcmp(pidStr, "s") == 0) {
        Serial.printf("setPoint: %.2f\n", val);
        setPoint = val;
      } else if (strcmp(pidStr, "N") == 0) {
        Serial.println("傳送PID初設值");
        notifyINIT();
      }
      break;
  }
}

void notifyClients() {
  // 底下是ArduinoJson V6的JSON文件變數宣告
  // StaticJsonDocument<48> doc;
  // 底下是ArduinoJson V7的JSON文件變數宣告
  JsonDocument doc;

  doc["d"] = "NTC";
  doc["v"] = (int)(temp * 100 + .5) / 100.0;

  char data[48];          // 儲存JSON字串的字元陣列
  serializeJson(doc, data);
  ws.textAll(data);        // 向所有連線的用戶端傳遞JSON字串
}

void notifyINIT() {
  // 底下是ArduinoJson V6的JSON文件變數宣告
  // StaticJsonDocument<96> doc;
  // 底下是ArduinoJson V7的JSON文件變數宣告
  JsonDocument doc;
  
  char buffer[35];
  snprintf(buffer, sizeof(buffer), "%.3f,%.3f,%.3f,%.3f",
           setPoint, kp, ki, kd);

  doc["d"] = "INIT";
  doc["v"] = buffer;

  char output[60];          // 儲存JSON字串的字元陣列
  serializeJson(doc, output);
  serializeJsonPretty(doc, Serial);  // 向序列埠輸出JSON內容
  Serial.println();
  ws.textAll(output);        // 向所有連線的用戶端傳遞JSON字串
}

void OLED() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_crox4hb_tf);
    u8g2.setCursor(0, 14);
    u8g2.print(ipAddr);  // 顯示IP位址
    u8g2.setCursor(0, 44);
    // 顯示目標溫度
    u8g2.print(String("SET: ") + String(setPoint, 2) + "\xb0C");
    u8g2.setCursor(0, 64);
    // 顯示感測溫度
    u8g2.print(String("TEMP: ") + String(temp, 2) + "\xb0C");
  } while ( u8g2.nextPage() );
}

void readSerial() {  // 讀取序列輸入字串的函式
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    sscanf(data.c_str(), "%f,%f,%f,%f", &setPoint, &kp, &ki, &kd);
    Serial.printf("setPoint= %.2f, kp= %.2f, ki= %.2f, kd= %.2f\n", setPoint, kp, ki, kd);
  }
}

float readTemp(float R0 = 10000.0, float beta = 3950.0) {
  int adc = analogRead(THERMO);
  float T0 = 25.0 + 273.15;
  float r = (adc * R0) / ((1<<ADC_BITS)-1 - adc);
  return 1 / (1 / T0 + 1 / beta * log(r / R0)) - 273.15;
}

float computePID(float in) {  // 計算PID
  static float errorSum = 0;  // 累計誤差
  static float prevError = 0;  // 前次誤差值
  float error = setPoint - in;  // 當前誤差
  float dt = INTERVAL / 1000.0;  // 間隔時間（秒）

  errorSum += error * dt;                // 積分：累計誤差
  errorSum = constrain(errorSum, 0, 255);   // 限制積分範圍

  float errorRate = (error - prevError) / dt;  // 微分：誤差程度變化
  prevError = error;  // 儲存本次誤差

  float output = kp * error + ki * errorSum + kd * errorRate;  // 計算PID
  output = constrain(output, 0, 255);                    // 限制輸出值範圍
  return output;
}

void setup() {
  Serial.begin(115200);
  pinMode(HEATER, OUTPUT);

  IPAddress ip;
  // 初始化SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("無法載入SPIFFS記憶體");
    return;
  }
  // 設置無線網路
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  ip = WiFi.localIP();
  ipAddr = WiFi.localIP().toString();
  Serial.printf("\nIP位址：%s\n", ipAddr.c_str());

  // 設置首頁
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.serveStatic("/favicon.ico", SPIFFS, "/www/favicon.ico");
  // 查無此頁
  server.onNotFound([](AsyncWebServerRequest * req) {
    req->send(404, "text/plain", "Not found");
  });

  ws.onEvent(onSocketEvent); // 附加事件處理程式
  server.addHandler(&ws);
  server.begin(); // 啟動網站伺服器
  Serial.println("HTTP伺服器開工了～");

  u8g2.begin();   // 初始化OLED顯示器
  // 設定類比輸入
  analogSetAttenuation(ADC_11db);  // 類比輸入上限3.6V
  analogReadResolution(ADC_BITS);  // 類比輸入位元解析度
  // 以下兩行適用於ESP32開發平台2.x版
  // ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_BITS);  // 設置PWM輸出
  // ledcAttachPin(HEATER, PWM_CHANNEL);      // 指定輸出腳
  // 底下敘述適用於ESP32開發平台3.x版
  ledcAttachChannel(HEATER, PWM_FREQ, PWM_BITS, PWM_CHANNEL); // 接腳, 頻率, 解析度, 通道
}

void loop() {
  static uint32_t prevTime = 0;   // 前次時間，宣告成「靜態」變數。
  uint32_t now = millis();        // 目前時間

  if (now - prevTime >= INTERVAL) {
    prevTime = now;

    temp = readTemp();      // 讀取溫度
    float power = computePID(temp);   // 計算PID
    // 底下敘述適用於ESP32開發平台2.x版
    // ledcWrite(PWM_CHANNEL, (int)power);  // 加熱
    // 底下敘述適用於ESP32開發平台3.x版
    ledcWrite(HEATER, (int)power); // 加熱
    // Serial.printf("%.2f,%.2f\n", setPoint, temp);

    OLED();  // 更新顯示畫面
    notifyClients();  // 向網路用戶端傳遞感測資料
  }

  ws.cleanupClients();
  readSerial();   // 讀取序列輸入參數
}
