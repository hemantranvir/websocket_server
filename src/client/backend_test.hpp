#ifndef _BACKEND_TEST_HPP__
#define _BACKEND_TEST_HPP__

#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "client_msg.hpp"
#include "websocket_client.hpp"

class BackendTest : public ::testing::Test
{
public:
    BackendTest();
    ~BackendTest();

    bool SendAndCheckCommand(const ClientMsg& msg);
    bool GetReceivedValue(const std::string& key, std::string& value);

protected:
    void SetUp();
    void TearDown();

    std::string ip;
    int port;

private:
    void GetServerParams();

    WebSocketClient* m_client;
};

#endif //_BACKEND_TEST_HPP__
