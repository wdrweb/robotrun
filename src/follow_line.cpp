#include <Arduino.h>
#include <Pololu3piPlus32U4.h>
#include "follow_line.h"
#include "navigate.h"

using namespace Pololu3piPlus32U4;

Motors motors;
Encoders encoders;
LineSensors lineSensors;
Buzzer buzzer;

SpecialCase specialCase;

const int baseSpeed = 60;        // 60, 250;
const int sensorThreshold = 400; // > 400 is black line

bool detectSpecialCases(uint16_t *sensorValues)
{

    static const int waitClicks = 80; // encoder clicks

    enum State
    {
        FOLLOWING,
        JUNCTION_DETECTED,
        LINE_LOST
    };
    static State state = FOLLOWING;
    static bool leftSeen = false;
    static bool rightSeen = false;

    lineSensors.readCalibrated(sensorValues);
    bool leftOuter = sensorValues[0] > sensorThreshold;
    bool rightOuter = sensorValues[4] > sensorThreshold;
    bool middle = max(sensorValues[1], max(sensorValues[2], sensorValues[3])) > sensorThreshold;

    switch (state)
    {
    case FOLLOWING:
        if (leftOuter || rightOuter)
        {
            // junction found start checking which one
            state = JUNCTION_DETECTED;
            leftSeen = leftOuter;
            rightSeen = rightOuter;
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight();
        }
        else if (!middle)
        {
            // line lost
            state = LINE_LOST;
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight();
        }
        break;

    case JUNCTION_DETECTED:
        // determine which junction it is

        motors.setSpeeds(baseSpeed, baseSpeed); // keep moving forward while checking for junction type

        while (!(max(abs(encoders.getCountsLeft()), abs(encoders.getCountsRight())) >= waitClicks))
        {
            // check for the line both outer sensors until the robot has moved enough to be sure
            lineSensors.readCalibrated(sensorValues);
            leftOuter = sensorValues[0] > sensorThreshold;
            rightOuter = sensorValues[4] > sensorThreshold;
            middle = max(sensorValues[1], max(sensorValues[2], sensorValues[3])) > sensorThreshold;

            if (leftOuter)
                leftSeen = true;
            if (rightOuter)
                rightSeen = true;
        }
        state = FOLLOWING; // set state back to following for next time

        // determine which junction it is
        if (leftSeen && rightSeen && middle)
        {
            specialCase = CROSSROAD;
            return true; // crossroad
        }
        else if (leftSeen && rightSeen && !middle)
        {
            specialCase = T_HEAD_ON;
            return true; // T head-on
        }
        else if (leftSeen && !rightSeen && middle)
        {
            specialCase = T_SECTION_LEFT;
            return true; // T-section left
        }
        else if (!leftSeen && rightSeen && middle)
        {
            specialCase = T_SECTION_RIGHT;
            return true; // T-section right
        }
        else if (leftSeen && !rightSeen && !middle)
        {
            specialCase = RIGHT_TURN_LEFT;
            return true; // right angle left
        }
        else if (!leftSeen && rightSeen && !middle)
        {
            specialCase = RIGHT_TURN_RIGHT;
            return true; // right angle right
        }

        break;

    case LINE_LOST:
        if (middle || leftOuter || rightOuter)
        {
            // line found again, just a gap or fluke
            state = FOLLOWING;
        }
        else if (max(abs(encoders.getCountsLeft()), abs(encoders.getCountsRight())) >= 0)
        { // if after some movement the line is not found, the line has ended
            state = FOLLOWING;
            specialCase = LINE_ENDED;
            return true; // line ended
        }
        break;
    }

    return false; // no special case detected
}

// proportional gain
const float kp = 0.04; // 0.05, 0.17;
// integral gain, not necessary for line following.
const float ki = 0;
// derivative gain to dampen oscillations
const float kd = 0.0005; // 0.03;

bool followLine(uint16_t *sensorValues)
{
    unsigned long lastTime = 0;
    float dt = 0.01; // 10ms loop time
    int lastError = 0;
    float integral = 0;
    float derivative = 0;
    motors.setSpeeds(baseSpeed, baseSpeed); // start moving forward

    while (true)
    {
        if (millis() - lastTime > dt * 1000) // overflow
        {                                    // 10ms interval
            // follow line with PID
            lastTime = millis();
            lineSensors.readCalibrated(sensorValues);
            int error = lineSensors.readLineBlack(sensorValues) - 2000; // error is distance line to center.
            integral = integral + error * dt;
            derivative = (error - lastError) / dt;
            lastError = error; // store for calculating next derivative
            float output = kp * error + ki * integral + kd * derivative;
            int leftSpeed = constrain((int)round(baseSpeed + output), -400, 400); // use lower constraints when the bot is to aggresive.
            int rightSpeed = constrain((int)round(baseSpeed - output), -400, 400);
            motors.setSpeeds(leftSpeed, rightSpeed);
            // check for special cases
            bool specialCaseDetected = detectSpecialCases(sensorValues);

            if (specialCaseDetected) // if a special case is detected, exit
            {
                return true;
            }
        }
    }
}

void followLineSetup() // calibrate sensors and prepare for line following
{
    delay(1000);
    turnDegrees(85); // turns 90 degrees. and minus 5 degrees to compansate for drift
    motors.setSpeeds((baseSpeed / 2), (baseSpeed / 2));
    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();
    // calibrating happens when the robot is moving, so it sees black and white.
    for (int i = 0; i < 60; i++)
    {
        lineSensors.calibrate();
    }
    motors.setSpeeds(0, 0);
    int leftCountsTarget = abs(encoders.getCountsLeft()); // store how much the wheels have turned
    int rightCountsTarget = abs(encoders.getCountsRight());
    turnDegrees(-180);
    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();

    motors.setSpeeds((baseSpeed / 2), (baseSpeed / 2));

    // the right wheel has turn as much as the left wheel did before, same goes for the left wheel, to make sure the robot is back with the same orientation.
    while (true)
    {
        int leftCounts = abs(encoders.getCountsLeft());
        int rightCounts = abs(encoders.getCountsRight());
        if (leftCounts >= rightCountsTarget)
        {
            motors.setLeftSpeed(0);
        }
        if (rightCounts >= leftCountsTarget)
        {
            motors.setRightSpeed(0);
        }
        if (leftCounts >= rightCountsTarget && rightCounts >= leftCountsTarget)
        {
            break; // both wheels have moved enough
        }
    }

    turnDegrees(88); // turns 90 minus 2 degrees to compansate for drift
    motors.setSpeeds(0, 0);
}
