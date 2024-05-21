#ifndef _FOO_H_
#define _FOO_H_
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void countUp(void);
int powerOut(int pwm);

#ifdef __cplusplus
}
#endif

#endif  // _FOO_H_