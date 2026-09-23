#include "navigate.h"
#include "follow_line.h"
#include <Pololu3piPlus32U4.h>
#include <Arduino.h>
#include <math.h>
#include "drive_distance.h"

using namespace Pololu3piPlus32U4;

extern Encoders encoders;
extern Motors motors;
extern LineSensors lineSensors;
extern Buzzer buzzer;

void turnDegrees(int degrees)
{
    int dir = (degrees >= 0) ? 1 : -1;
    int countsPerWheel = (int)round((abs(degrees) / 360.0) * 956); // 800 counts per full turn (adjusted from 956.5 for better tuning)

    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();
    motors.setSpeeds(baseSpeed * dir, -baseSpeed * dir); // turn in place
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
            break; // both wheels have turned enough
        }
    }
    motors.setSpeeds(0, 0); // stop after turn
}




bool navigate()
{
    int tSectionLeftCount = 0; // used for detecting the warehouse. 
    while (true)
    {
        uint16_t sensorValues[5];
        followLine(sensorValues); // follows the line until it detects a special case. specialCase is updated in followLine 
        motors.setSpeeds(0, 0);


        switch (specialCase)
        { // priority; straight ahead > right turn > left turn. in case of lineend turn 180 degrees. 
        case LINE_ENDED:
            motors.setSpeeds(0, 0);
            turnDegrees(180);
            driveDistance(40); // stop at the end of the line
            tSectionLeftCount = 0;
            break;
        case RIGHT_TURN_LEFT:
            motors.setSpeeds(0, 0);

            if (tSectionLeftCount > 2)
            {   
                buzzer.playFrequency(300, 200, 15); // beep when arrived
                return true; // a left turn after 3 or more T_SECTION_LEFT == arrived in warehouse at A0
            }
            turnDegrees(-80); // 90 - 10 to compensate for drift.
            // driveDistance(30);

            driveDistance(30);
            tSectionLeftCount = 0;
            break;
        case RIGHT_TURN_RIGHT:
            motors.setSpeeds(0, 0);
            // driveDistance(30);
            turnDegrees(80);
            driveDistance(30);
            tSectionLeftCount = 0;

            break;
        case T_HEAD_ON:
            motors.setSpeeds(0, 0);
            // driveDistance(30);
            turnDegrees(85);
            driveDistance(30);
            tSectionLeftCount = 0;
            break;
        case CROSSROAD:
            motors.setSpeeds(0, 0);
            turnDegrees(85);

            //check if this is actually a crossroad, of if it is the start/stop point. 
            lineSensors.readCalibrated(sensorValues);
            if (sensorValues[0] > sensorThreshold) // left sensor still reads black after turning, so this is the start/stop point. 
            {
                turnDegrees(-85);
                // Execute stop flow immediately; specialCase is recalculated in the next loop.
                driveDistance(60);
                turnDegrees(180);
                tSectionLeftCount = 0;
                buzzer.playFrequency(300, 200, 15);
                return true;
            }

            tSectionLeftCount = 0;
            break;

        case T_SECTION_LEFT:
            turnDegrees(0);
            tSectionLeftCount++;
            break;
        case T_SECTION_RIGHT:
            turnDegrees(85);
            tSectionLeftCount = 0;
            break;
        }
    }
}
