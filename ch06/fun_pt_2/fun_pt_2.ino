int add(int a, int b){  // 定義「a加b」函式
   return a+b;
}  

void setup() {
  Serial.begin(115200);

  int (*pt)(int, int);   // 宣告函式指標，傳回值與參數的型態都要與函式相符。
  pt = add;          // 儲存函式的位址
  int ans = (*pt)(2, 3);  // 間接執行函式
  Serial.printf("ans= %d\n", ans);
}

void loop() { }
