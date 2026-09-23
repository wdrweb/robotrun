#include <Arduino.h>
//#include "get_order.h"
#include "follow_line.h"
#include "navigate.h"
#include "warehouse.h"



void setup()
{
  followLineSetup(); // used to calibrate line sensors. 
}

void loop()
{
  getOrder();
  driveDistance(100); // drive a bit forward to position the robot on top of the line
  navigate(); //stops in warehouse at A4, direction 3, to the left
  retrieveOrder(); // navigate to coordinates and pick up products. ends at coordinate E0, direction 1, up
  navigate(); // exit warehouse and navigate to stoppoint.
}