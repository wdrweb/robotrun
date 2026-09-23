#include "sort_coordinates.h"

void sortCoordinates(int coords[][2], int count)
{
    // Sort by y descending first; if y is equal, sort by x descending.
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            bool shouldSwap = false;

            if (coords[i][1] < coords[j][1]) // sort Y
            {
                shouldSwap = true;
            }
            else if (coords[i][1] == coords[j][1] && coords[i][0] < coords[j][0]) // sort X
            {
                shouldSwap = true;
            }

            if (shouldSwap)
            {
                int tempX = coords[i][0];
                int tempY = coords[i][1];

                coords[i][0] = coords[j][0];
                coords[i][1] = coords[j][1];

                coords[j][0] = tempX;
                coords[j][1] = tempY;
            }
        }
    }
}
