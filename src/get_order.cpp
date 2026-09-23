#include <Arduino.h>
#include <Pololu3piPlus32U4.h>
#include "get_order.h"
#include "warehouse.h"

using namespace Pololu3piPlus32U4;

OLED display;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;

typedef enum
{
  MAIN_MENU,
  PRODUCT_MENU,
  COORDINATE_MENU
} MenuState;

const int coordinateCount = productCount;
int prodMenuCursLoc[4] = {0, 2, 6, 9}; // locations (back, x, y, delete)

MenuState menuState = MAIN_MENU;
int mainMenuIndex = 1;
int cursorLocation = 0;
bool menuEntry = true; // entry variable for the menu switch cases.
bool orderConfirmed = false;

inline void resetDisplay()
{
  display.setLayout11x4();
  display.gotoXY(0, 0);
}

void initUnsetCoordinates(int productIndex) // if a coordinate is -1, -1 this sets to 0,0
{
  if (productIndex < 1 || productIndex > coordinateCount)
  {
    return;
  }

  int &xCoordinate = coordinates[productIndex - 1][0];
  int &yCoordinate = coordinates[productIndex - 1][1];

  if (xCoordinate == -1 && yCoordinate == -1)
  {
    xCoordinate = 0;
    yCoordinate = 0;
  }
}

void drawMainMenu(int mainMenuIndex)
{
  Serial.print(mainMenuIndex);
  char xChar = '-';
  char yChar = '-';

  if (mainMenuIndex >= 1 && mainMenuIndex <= coordinateCount) // products
  {
    int xCoordinate = coordinates[mainMenuIndex - 1][0];
    int yCoordinate = coordinates[mainMenuIndex - 1][1];
    xChar = xCoordinate == -1 ? '-' : 'A' + xCoordinate; // characters are stored as their ascii number, adding one gets the next character, 'A' + 1 = 'B'
    yChar = yCoordinate == -1 ? '-' : '0' + yCoordinate;
    resetDisplay();
    display.print(F("Product  :"));
    display.gotoXY(8, 0);
    display.print(mainMenuIndex);
    display.gotoXY(0, 1);
    char locationLine[12];
    sprintf(locationLine, "Loc:%c,%c", xChar, yChar);
    display.print(locationLine);
  }
  else if (mainMenuIndex == 0) // delete order
  {
    resetDisplay();
    display.print(F("Verwijder"));
    display.gotoXY(0, 1);
    display.print(F("Order"));
  }
  else if (mainMenuIndex == coordinateCount + 1) // confirm order
  {
    resetDisplay();
    display.print(F("Bevestig"));
    display.gotoXY(0, 1);
    display.print(F("Order"));
  }

  // UI navigation hints
  display.gotoXY(0, 3);
  display.print(F("<A  *B  C>"));
}

void moveProductCursor(int direction, int &cursorLocation)
{
  display.gotoXY(prodMenuCursLoc[cursorLocation], 1);
  display.print(' ');
  cursorLocation = (cursorLocation + direction + 4) % 4;
  display.gotoXY(prodMenuCursLoc[cursorLocation], 1);
  display.print('*');
}

void drawProductMenu(int productIndex)
{
  int xCoordinate = coordinates[productIndex - 1][0];
  int yCoordinate = coordinates[productIndex - 1][1];
  char xChar = xCoordinate == -1 ? '-' : 'A' + xCoordinate;
  char yChar = yCoordinate == -1 ? '-' : '0' + yCoordinate;

  resetDisplay();
  char topLine[12];
  sprintf(topLine, "\x7fX:%c Y:%c D", xChar, yChar); // \x7f is the back arrow
  display.print(topLine);
  display.gotoXY(0, 3);
  display.print(F("<A  *B  C>"));
}

void drawCoordinateMenu(int coordinate, bool isX) // menu used for editing coordinates
{
  resetDisplay();

  if (isX)
  {
    display.print(F("X Bewerken:"));
  }
  else
  {
    display.print(F("Y Bewerken:"));
  }

  display.gotoXY(0, 3);
  display.print(F("<A  *B  C>"));
}

void changeCoordinate(int productIndex, int direction, int isXY) // this function changes a coordinate up ot down and updates the display. x=0, y=1
{
  int *coordinate = &coordinates[productIndex][isXY];

  if (direction == -1)
  {
    direction = 4; // grid size 5x5
  }

  *coordinate = (*coordinate + direction) % 5;
  display.gotoXY(5, 1); // coordinate location in the coordinate menu

  if (isXY == 0)
  {
    char letter = 'A' + *coordinate;
    display.print(letter);
  }
  else
  {
    display.print(*coordinate);
  }
}
void showDeletionConfirmation(bool isOrder)
{
  resetDisplay();
  if (isOrder)
  {
    display.print(F("Order"));
  }
  else
  {
    display.print(F("Product"));
  }
  display.gotoXY(0, 1);
  display.print(F("verwijderd"));
  delay(1000);
  resetDisplay();
}

void showOrderConfirmation()
{
  resetDisplay();
  display.print(F("Order"));
  display.gotoXY(0, 1);
  display.print(F("bevestigd"));
  delay(1000);
  resetDisplay();
}

void mainMenu(int &mainMenuIndex, MenuState &menuState, int &cursorLocation, bool &entry)
{
  static int amountOfMainMenuItems = coordinateCount + 2; // delete + products + confirm

  if (entry)
  {
    display.clear();
    drawMainMenu(mainMenuIndex);
    entry = false;
  }

  if (buttonA.getSingleDebouncedPress()) // move back one page
  {
    mainMenuIndex = (mainMenuIndex + amountOfMainMenuItems - 1) % amountOfMainMenuItems;
    drawMainMenu(mainMenuIndex);
  }

  if (buttonC.getSingleDebouncedPress()) // move forward one page
  {
    mainMenuIndex = (mainMenuIndex + 1) % amountOfMainMenuItems;
    drawMainMenu(mainMenuIndex);
  }

  if (buttonB.getSingleDebouncedPress()) // select page
  {
    if (mainMenuIndex == 0)
    {
      for (int i = 0; i < coordinateCount; i++) // delete all products
      {
        coordinates[i][0] = -1;
        coordinates[i][1] = -1;
      }
      showDeletionConfirmation(true);
      display.clear();
      entry = true;
    }
    else if (mainMenuIndex == coordinateCount + 1) // confirm order
    {
      showOrderConfirmation();
      orderConfirmed = true;
      entry = true;
    }
    else
    {
      menuState = PRODUCT_MENU; // select product page
      cursorLocation = 0;
      initUnsetCoordinates(mainMenuIndex);
      entry = true;
    }
  }
}

void productMenu(int &mainMenuIndex, MenuState &menuState, int &cursorLocation, bool &entry)
{ // this menu is used for editing a product, it has 4 options, back, x coordinate, y coordinate and delete product
  if (entry)
  {
    display.clear();
    drawProductMenu(mainMenuIndex);
    moveProductCursor(0, cursorLocation);
    entry = false;
  }

  if (buttonA.getSingleDebouncedPress())
  {
    moveProductCursor(-1, cursorLocation);
  }
  else if (buttonC.getSingleDebouncedPress())
  {
    moveProductCursor(1, cursorLocation);
  }
  else if (buttonB.getSingleDebouncedPress())
  { // select option
    if (cursorLocation == 0)
    {
      menuState = MAIN_MENU;
      entry = true;
    }
    else if (cursorLocation == 3)
    {
      coordinates[mainMenuIndex - 1][0] = -1;
      coordinates[mainMenuIndex - 1][1] = -1;
      showDeletionConfirmation(false);
      menuState = MAIN_MENU;
      entry = true;
    }
    else
    {
      menuState = COORDINATE_MENU;
      entry = true;
    }
  }
}

void coordinateMenu(int &mainMenuIndex, MenuState &menuState, int &cursorLocation, bool &entry)
{ // this menu is used for editing a coordinate
  if (entry)
  {
    display.clear();
    drawCoordinateMenu(coordinates[mainMenuIndex - 1][cursorLocation - 1], cursorLocation == 1);
    changeCoordinate(mainMenuIndex - 1, 0, cursorLocation - 1); // write coordinate to display
    entry = false;
  }

  if (buttonA.getSingleDebouncedPress())
  {
    changeCoordinate(mainMenuIndex - 1, -1, cursorLocation - 1); // x or y is selected based on the cursor location. 
  }

  if (buttonC.getSingleDebouncedPress())
  {
    changeCoordinate(mainMenuIndex - 1, 1, cursorLocation - 1);
  }

  if (buttonB.getSingleDebouncedPress())
  {
    menuState = PRODUCT_MENU;
    entry = true;
  }
}

void setupOrder()
{ // initialize variables and display.
  orderConfirmed = false;
  menuState = MAIN_MENU;
  mainMenuIndex = 1;
  cursorLocation = 0;
  menuEntry = true;

  for (int i = 0; i < coordinateCount; i++)
  {
    coordinates[i][0] = -1;
    coordinates[i][1] = -1;
  }

  display.init();
  display.clear();
  display.setLayout11x4();
}

void orderMenu() // this function is called in a loop until the order is confirmed, it handles the menu navigation and drawing.
{
  switch (menuState)
  {
  case MAIN_MENU:
    mainMenu(mainMenuIndex, menuState, cursorLocation, menuEntry);
    break;

  case PRODUCT_MENU:
    productMenu(mainMenuIndex, menuState, cursorLocation, menuEntry);
    break;

  case COORDINATE_MENU:
    coordinateMenu(mainMenuIndex, menuState, cursorLocation, menuEntry);
    break;
  }
}

bool getOrder()
{
  setupOrder();
  while (!orderConfirmed)
  {
    orderMenu();
    delay(10);
  }

  return true;
}

int getOrderCount()
{
  return coordinateCount;
}