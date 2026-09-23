#include "drive_distance.h"
#include <Pololu3piPlus32U4.h>
#include <Arduino.h>

using namespace Pololu3piPlus32U4;

Encoders encoders;
Motors motors;
const int baseSpeed = 60;

void driveDistance(int distance_mm)
{ // this function does not correct for any lateral movement caused by motorspeed being out of sync. it does preserve orientation. 
    // wheel diameter = 32mm, cpr = 29.86 × 12 ≈ 358.3,
    int countsPerWheel = abs((int)round((distance_mm / (PI * 32)) * 358.3));
    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();
    int speed = (distance_mm >= 0) ? baseSpeed : -baseSpeed;
    motors.setSpeeds(speed, speed); // move forward
    while (true)
    {
        int leftCounts = abs(encoders.getCountsLeft());
        int rightCounts = abs(encoders.getCountsRight());
        if (leftCounts >= countsPerWheel)
        {
            motors.setLeftSpeed(0);
        }
        if (rightCounts >= countsPerWheel)
        {
            motors.setRightSpeed(0);
        }
        if (leftCounts >= countsPerWheel && rightCounts >= countsPerWheel)
        {
            break; // both wheels have moved enough
        }
    }
    motors.setSpeeds(0, 0); // stop after moving
}
