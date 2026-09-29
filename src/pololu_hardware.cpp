#include "pololu_hardware.h"
#include "drive_distance.h"
#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

int16_t PololuEncoders::getCountsLeft() { return Encoders::getCountsLeft(); }
int16_t PololuEncoders::getCountsRight() { return Encoders::getCountsRight(); }
int16_t PololuEncoders::getCountsAndResetLeft() { return Encoders::getCountsAndResetLeft(); }
int16_t PololuEncoders::getCountsAndResetRight() { return Encoders::getCountsAndResetRight(); }

void PololuMotors::setSpeeds(int16_t left, int16_t right) { Motors::setSpeeds(left, right); }
void PololuMotors::setLeftSpeed(int16_t speed) { Motors::setLeftSpeed(speed); }
void PololuMotors::setRightSpeed(int16_t speed) { Motors::setRightSpeed(speed); }

void driveDistance(int distance_mm)
{
    PololuEncoders enc;
    PololuMotors mot;
    driveDistance(distance_mm, enc, mot);
}
