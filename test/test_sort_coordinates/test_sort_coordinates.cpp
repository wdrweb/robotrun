#include <gtest/gtest.h>
#include "sort_coordinates.h"

// it should sort y from high to low, then x from high to low if y is equal
TEST(SortTest, YDescending) {
    int coords[3][2] = {{0,1},{2,0},{1,1}};
    sortCoordinates(coords, 3);
    EXPECT_EQ(coords[0][1], 1);
    EXPECT_EQ(coords[1][1], 1);
    EXPECT_EQ(coords[2][1], 0);
    
}
TEST(SortTest, XDescendingOnEqualY){
    int coords[3][2] = {{0,1},{2,0},{1,1}};
    sortCoordinates(coords, 3);
    EXPECT_EQ(coords[0][1], 1);
    EXPECT_EQ(coords[1][1], 1);
    EXPECT_EQ(coords[2][1], 0);
    EXPECT_EQ(coords[0][0], 1);
    EXPECT_EQ(coords[1][0], 0);
    EXPECT_EQ(coords[2][0], 2);

}
TEST(SortTest, AlreadySorted){
    int coords[3][2] = {{1,1},{0,1},{2,0}};
    sortCoordinates(coords, 3);
    EXPECT_EQ(coords[0][1], 1);
    EXPECT_EQ(coords[1][1], 1);
    EXPECT_EQ(coords[2][1], 0);
    EXPECT_EQ(coords[0][0], 1);
    EXPECT_EQ(coords[1][0], 0);
    EXPECT_EQ(coords[2][0], 2);

}
TEST(SortTest, SingleElement){
    int coords[1][2] = {{1,1}};
    sortCoordinates(coords, 1);
    EXPECT_EQ(coords[0][1], 1);
    EXPECT_EQ(coords[0][0], 1);
}
TEST(SortTest, EmptyArray){
    int coords[0][2] = {};
    sortCoordinates(coords, 0);

}
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}