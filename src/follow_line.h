#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H
#include <stdint.h>

typedef enum
{
    LINE_ENDED = 0,
    RIGHT_TURN_LEFT = 1,
    RIGHT_TURN_RIGHT = 2,
    T_HEAD_ON = 3,
    CROSSROAD = 4,
    T_SECTION_LEFT = 5,
    T_SECTION_RIGHT = 6 
} SpecialCase;

extern SpecialCase specialCase;
extern const int baseSpeed;
extern const int sensorThreshold;
extern 

bool followLine(uint16_t* sensorValues);
void followLineSetup();

#endif // FOLLOW_LINE_H