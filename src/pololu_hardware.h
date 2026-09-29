#pragma once
#include "robot_interface.h"

class PololuEncoders : public IEncoders {
public:
    int16_t getCountsLeft() override;
    int16_t getCountsRight() override;
    int16_t getCountsAndResetLeft() override;
    int16_t getCountsAndResetRight() override;
};

class PololuMotors : public IMotors {
public:
    void setSpeeds(int16_t left, int16_t right) override;
    void setLeftSpeed(int16_t speed) override;
    void setRightSpeed(int16_t speed) override;
};


void driveDistance(int distance_mm);
