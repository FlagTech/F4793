#include <WiFi.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <esp32_can.h>
#include <esp32_obd2.h>
#define INTERVAL 1000
#define CRX_PIN  16
#define CTX_PIN  17
#define LED_PIN  2

const char* ssid = "ESP32-OBDII";
const char* password = "87654321";

AsyncWebServer server(80); // 建立HTTP伺服器物件
AsyncWebSocket ws("/ws");  // 建立WebSocket物件

void onSocketEvent(AsyncWebSocket *server,
                   AsyncWebSocketClient *client,
                   AwsEventType type,
                   void *arg,
                   uint8_t *data,
                   size_t len)
{
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
      break;
  }
}

void notifyClients() {
  JsonDocument doc;

  doc["spd"] = OBD2.pidRead(VEHICLE_SPEED);
  doc["rpm"] = OBD2.pidRead(ENGINE_RPM);
  //  doc["spd"] = 86.45;
  //  doc["rpm"] = 1984;
  String output;
  serializeJson(doc, output);
  ws.textAll(output);        // 向所有連線的用戶端傳遞JSON字串
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  if (!SPIFFS.begin(true)) {
    Serial.println("無法載入SPIFFS記憶體");
    return;
  }

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // 設置首頁
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.serveStatic("/favicon.ico", SPIFFS, "/www/favicon.ico");
  server.serveStatic("/fonts/SevenSegment.woff2", SPIFFS, "/www/fonts/SevenSegment.woff2");
  // 查無此頁
  server.onNotFound([](AsyncWebServerRequest * req) {
    req->send(404, "text/plain", "Not found");
  });

  ws.onEvent(onSocketEvent); // 附加事件處理程式
  server.addHandler(&ws);
  server.begin(); // 啟動網站伺服器
  Serial.println("HTTP伺服器開工了～");

  // 處理OBD(CAN)通訊
  CAN0.setCANPins((gpio_num_t)CRX_PIN, (gpio_num_t)CTX_PIN);
  Serial.println("嘗試連線到OBD2 CAN匯流排…");
  while (1) {
    // 嘗試連線到OBD2 CAN匯流排…
    if (!OBD2.begin()) {
      // 無法連線！
      Serial.println("無法連線！");
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    } else {
      //連線成功！
      digitalWrite(LED_PIN, HIGH);
      break;
    }
  }
}

void loop() {
  static uint32_t prevTime = 0;   // 前次時間，宣告成「靜態」變數。
  uint32_t now = millis();        // 目前時間

  if (now - prevTime >= INTERVAL) {
    prevTime = now;

    notifyClients();  // 向網路用戶端傳遞感測資料
  }

  ws.cleanupClients();
}
