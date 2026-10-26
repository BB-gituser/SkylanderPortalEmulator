#ifndef JOY_H
#define JOY_H
#include <stdbool.h>

#define TIME_WAIT 200

typedef struct {
    int right;
    int left;
    int bottom;
    int top;
    int press;
} JoystickStat;

JoystickStat joystick_get();

#endif