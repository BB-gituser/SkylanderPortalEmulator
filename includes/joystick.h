#ifndef JOY_H
#define JOY_H
#include <stdbool.h>

typedef struct {
    int right;
    int left;
    int bottom;
    int top;
    int press;
} JoystickStat;

JoystickStat joystick_get();

#endif