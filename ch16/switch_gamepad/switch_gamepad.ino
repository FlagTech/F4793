#include <switch_ESP32.h>
#include <Wire.h>
#include <WiiClassic.h>

NSGamepad Gamepad;
WiiClassic wii = WiiClassic();

void setup() {
  Wire.begin();
  wii.begin();
  wii.update();

  Gamepad.begin();
  USB.begin();
}

void loop() {
  wii.update();
  
  if (wii.leftShoulderPressed())
    Gamepad.press(NSButton_RightTrigger);
  else
    Gamepad.release(NSButton_RightTrigger);

  if (wii.rightShoulderPressed())
    Gamepad.press(NSButton_LeftTrigger);
  else
    Gamepad.release(NSButton_LeftTrigger);

  if (wii.aPressed())
    Gamepad.press(NSButton_A);
  else
    Gamepad.release(NSButton_A);

  if (wii.bPressed())
    Gamepad.press(NSButton_B);
  else
    Gamepad.release(NSButton_B);

  if (wii.xPressed())
    Gamepad.press(NSButton_X);
  else
    Gamepad.release(NSButton_X);

  if (wii.yPressed())
    Gamepad.press(NSButton_Y);
  else
    Gamepad.release(NSButton_Y);

  if (wii.homePressed())
    Gamepad.press(NSButton_Home);
  else
    Gamepad.release(NSButton_Home);

  if (wii.startPressed())   // + (Start)
    Gamepad.press(NSButton_Plus);
  else
    Gamepad.release(NSButton_Plus);

  if (wii.selectPressed())   // - (Select)
    Gamepad.press(NSButton_Minus);
  else
    Gamepad.release(NSButton_Minus);

  if (wii.lzPressed())
    Gamepad.press(NSButton_LeftThrottle);
  else
    Gamepad.release(NSButton_LeftThrottle);

  if (wii.rzPressed())
    Gamepad.press(NSButton_RightThrottle);
  else
    Gamepad.release(NSButton_RightThrottle);

  // 十字鍵
  Gamepad.dPad(wii.upDPressed(), wii.downDPressed(),
               wii.leftDPressed(), wii.rightDPressed() );


  // 左右搖桿
  uint8_t leftX = map(wii.leftStickX(), 0, 63, 0, 255);
  uint8_t leftY = map(wii.leftStickY(), 0, 63, 255, 0);  // 上下相反
  uint8_t rightX = map(wii.rightStickX(), 0, 31, 0, 255);
  uint8_t rightY = map(wii.rightStickY(), 0, 31, 255, 0);  // 上下相反
  
  Gamepad.leftXAxis(leftX);
  Gamepad.leftYAxis(leftY);
  Gamepad.rightXAxis(rightX);
  Gamepad.rightYAxis(rightY);
  
  Gamepad.loop();
  delay(20);
}
