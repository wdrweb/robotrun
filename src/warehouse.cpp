#include "warehouse.h"
#include <Arduino.h>
#include "follow_line.h"
#include "get_order.h"
#include "navigate.h"
#include "sort_coordinates.h"
#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

int coordinates[productCount][2];


int currentDirection = 3; // 0 = up, 1 = right, 2 = down, 3 = left.
int currentX = 0;         // start at A4 (0,4)
int currentY = 4;

int actualCoordinateCount;
uint16_t sensorValues[5];

extern Buzzer buzzer;
extern Motors motors;

void setupWarehouse()
{
    actualCoordinateCount = 0;
    sortCoordinates(coordinates, productCount);

    // count how many coordinates there are, so the bot doesnt try to pick up -1,-1 coordinates
    for (int i = 0; i < productCount; i++)
    {
        if (coordinates[i][0] != -1 && coordinates[i][1] != -1)
        {
            actualCoordinateCount++;
        }
    }
}


void navigateToCoordinate(int x, int y)
{
    int xDiff = x - currentX;
    int yDiff = y - currentY;
    int xDirection = xDiff > 0 ? 1 : 3; // 1 for right, 3 for left
    int yDirection = yDiff > 0 ? 0 : 2; // 0 for up, 2 for down

    // first move to the correct x coordinate, then move to the correct y coordinate
    int turnValue;
    if (xDiff != 0)
    {
        
        turnValue = ((xDirection - currentDirection + 4) % 4) * 90;
        if (turnValue == 270) 
        {
            turnValue = -90;
        }
        turnDegrees(turnValue); // turn to the correct direction
        currentDirection = xDirection;
        for (int i = 0; i < abs(xDiff); i++)
        {
            followLine(sensorValues); // followLine drives until it detects a special case, so in a forloop it will drive one coordinate at a time.
            // Move past the detected intersection so the next step does not re-detect the same node.
            motors.setSpeeds(baseSpeed, baseSpeed);
            driveDistance(15);
        }
    }

    if (yDiff != 0)
    {   
        turnValue = ((yDirection - currentDirection + 4) % 4) * 90;
        if (turnValue == 270) 
        {
            turnValue = -90;
        }
        turnDegrees(turnValue);
        currentDirection = yDirection;
        for (int i = 0; i < abs(yDiff); i++)
        {
            followLine(sensorValues);
            // Move past the detected intersection so the next step does not re-detect the same node.
            motors.setSpeeds(baseSpeed, baseSpeed);
            driveDistance(15);
        }
    }
    currentX = x;
    currentY = y;
    motors.setSpeeds(0, 0);

    // beep when arrived
}

void navigateToExit()
{
    navigateToCoordinate(4, 0);    
    int turnValue = ((0 - currentDirection + 4) % 4) * 90; 
    if (turnValue == 270) {
        turnValue = -90;
    }                 // E0
    turnDegrees(turnValue); // turn to up
    specialCase = LINE_ENDED; // to make sure it is not CROSSROAD when the while loop starts. It does not matter if it is LINE_ENDED or not.
    while (specialCase != CROSSROAD)
    {
        followLine(sensorValues);
    }
    buzzer.playFrequency(200, 200, 15);
    turnDegrees(90);
}
void retrieveOrder()
{
    setupWarehouse();
    for (int i = 0; i < actualCoordinateCount; i++)
    {
        navigateToCoordinate(coordinates[i][0], coordinates[i][1]);
        buzzer.playFrequency(200, 200, 15);
        delay(1000); // wait for 1 second to simulate picking up the product
    }
    navigateToExit();
    currentDirection = 3; // 0 = up, 1 = right, 2 = down, 3 = left.
    currentX = 0;         // start at A4 (0,4)
    currentY = 4;
}
