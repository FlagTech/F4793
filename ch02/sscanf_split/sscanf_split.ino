void setup() {
  char name[30], size;  // 接收字串和字元型態資料的變數
  int price;            // 接收整數型態資料的變數
  char drink[] = "Latte,M,60"; // 逗號分隔資料（字串）

  sscanf(drink, "%[^,],%c,%d", name, &size, &price); // 讀取並以逗號分割資料
  Serial.begin(115200);
  Serial.printf("品名：%s\t容量：%c\t價格：%d\n", name, size, price);
}

void loop() { }
