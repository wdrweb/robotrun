#include "drive_distance.h"
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cmath>
#include <cstdlib>
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

const int baseSpeed = 60;

void driveDistance(int distance_mm, IEncoders &enc, IMotors &mot)
{ // this function does not correct for any lateral movement caused by motorspeed being out of sync. it does preserve orientation.
    // wheel diameter = 32mm, cpr = 29.86 x 12 ~= 358.3,
    int countsPerWheel = abs((int)round((distance_mm / (PI * 32)) * 358.3));
    enc.getCountsAndResetLeft();
    enc.getCountsAndResetRight();
    int speed = (distance_mm >= 0) ? baseSpeed : -baseSpeed;
    mot.setSpeeds((int16_t)speed, (int16_t)speed); // move forward
    while (true)
    {
        int leftCounts = abs((int)enc.getCountsLeft());
        int rightCounts = abs((int)enc.getCountsRight());
        if (leftCounts >= countsPerWheel)
        {
            mot.setLeftSpeed(0);
        }
        if (rightCounts >= countsPerWheel)
        {
            mot.setRightSpeed(0);
        }
        if (leftCounts >= countsPerWheel && rightCounts >= countsPerWheel)
        {
            break; // both wheels have moved enough
        }
    }
    mot.setSpeeds(0, 0); // stop after moving
}
