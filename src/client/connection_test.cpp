#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "backend_test.hpp"

TEST_F(BackendTest, LogonLogoutTest)
{
    //Send and check reply
    EXPECT_TRUE(SendAndCheckCommand(ClientMsg("{\"command\":\"INITID\",\"id\":\"0\"}",
                                              kRecv,
                                              "{\"command\":\"INITID\",\"status\":\"OK\"}")));

}
