#pragma once
#include <stdint.h>


class IEncoders {
public:
    virtual ~IEncoders() = default;
    virtual int16_t getCountsLeft() = 0;
    virtual int16_t getCountsRight() = 0;
    virtual int16_t getCountsAndResetLeft() = 0;
    virtual int16_t getCountsAndResetRight() = 0;
};

class IMotors {
public:
    virtual ~IMotors() = default;
    virtual void setSpeeds(int16_t left, int16_t right) = 0;
    virtual void setLeftSpeed(int16_t speed) = 0;
    virtual void setRightSpeed(int16_t speed) = 0;
};
