#include "pch.h"
#include "../ConsoleApplication1/CCC.h"
#include "../ConsoleApplication1/CCC.cpp"


TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
  EXPECT_EQ(7, CCC::add(2, 5));
  EXPECT_EQ(8, CCC::add(2, 5));

}