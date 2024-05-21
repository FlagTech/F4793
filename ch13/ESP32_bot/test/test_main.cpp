#include <Arduino.h>
#include <unity.h>

#include "foo.h"  // 引用被測試對象的程式庫

extern int counter;  // 取用定義在他處（此例為foo.c）的全域變數
const uint8_t LED_PIN = 5;
String host;  // 宣告字串變數

void setUp() {  // 注意U大寫，裡面寫入要在單元測試之前執行的敘述。
  pinMode(LED_PIN, OUTPUT);  // pinMode()敘述也可以寫在這裡
  host = "swf.com.tw";       // 設定字串變數值
}

void tearDown() { digitalWrite(LED_PIN, HIGH); }  // 單元測試結束後執行的工作

void test_LED_ON() {  // 測試數位腳是否如預期般輸出
  digitalWrite(LED_PIN, LOW);
  bool val = digitalRead(LED_PIN);
  TEST_ASSERT_FALSE_MESSAGE(val, "LED is OFF!");  // val值是false，測試過關。
}

void test_powerOut() {
  int pwm = -200;  // 測試用的實際PWM值
  int power = powerOut(pwm);
  // 測試powerOut()的輸出值是否介於-100~100。
  TEST_ASSERT_INT_WITHIN(100, 0, power);  // 測試結果：過關
}

void test_countUp() {
  int initial = counter;  // 儲存當前的「計數器」值
  countUp();              // 呼叫「計數器+1」函式
  // 測試呼叫countUp()函式之後，count全域變數值是否+1。
  TEST_ASSERT_EQUAL_INT(initial + 1, counter);  // 預期值=之前的計數值+1
}

void test_substring() {
  TEST_ASSERT_EQUAL_STRING("swf", host.substring(0, 3).c_str());
}

void test_bits() {   // 測試兩個整數的特定位元範圍值
  int act = 0x1234;  // 實際值
  int exp = 0x1200;  // 期待值
  TEST_ASSERT_BITS(0xFF00, exp, act);  // 進行位元值測試：過關。
}

void test_ip_number() {
  IPAddress ip(192 | (168 << 8) | (1 << 16) | (2 << 24));
  TEST_ASSERT_EQUAL(ip[0], 192);
  TEST_ASSERT_EQUAL(ip[1], 168);
  TEST_ASSERT_EQUAL(ip[2], 1);
  TEST_ASSERT_EQUAL(ip[3], 2);
}

void test_ip_array() {
  uint8_t const ip_addr_array[] = {192, 168, 1, 2};
  IPAddress ip(ip_addr_array);

  TEST_ASSERT_EQUAL(129, ip[0]);  // 測試結果：兩個數值不同，失敗。
  TEST_ASSERT_EQUAL(168, ip[1]);
  TEST_ASSERT_EQUAL(1, ip[2]);
  TEST_ASSERT_EQUAL(2, ip[3]);
}

void setup() {
  UNITY_BEGIN();             // 開始單元測試
  RUN_TEST(test_LED_ON);     // 測試數位輸出：過關
  RUN_TEST(test_powerOut);   // 測試PWM輸出範圍：過關
  RUN_TEST(test_countUp);    // 測試「計數值+1」：過關
  RUN_TEST(test_substring);  // 測試子字串值：過關
  RUN_TEST(test_ip_number);  // 測試IP數字：過關
  RUN_TEST(test_ip_array);   // 測試IP數字：其中一個不過關
  RUN_TEST(test_bits);       // 測試數字位元：過關
  UNITY_END();               // 單元測試結束
}

void loop() {}
