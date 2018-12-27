#include "backend_test.hpp"

BackendTest::BackendTest()
{
}

BackendTest::~BackendTest()
{
}

void BackendTest::SetUp()
{
    std::cout << "[BackendTest] " << "Setup Called" << std::endl;

    GetServerParams();

    m_client =  new WebSocketClient(ip, port, std::cout);

    std::cout << "[BackendTest] " << "Setup Finished" << std::endl;
}

void BackendTest::TearDown()
{
    std::cout << "[BackendTest] " << "TearDown Called" << std::endl;

    if (m_client)
    {
        m_client->Join();
        delete m_client;
        m_client = nullptr;
    }

    std::cout << "[BackendTest] " << "TearDown Finished" << std::endl;
}

bool BackendTest::SendAndCheckCommand(const ClientMsg& msg)
{
    bool ret = true;
    ret &= m_client->SendAndCheckCommand(msg);
    return ret;
}

bool BackendTest::GetReceivedValue(const std::string& key, std::string& value)
{
    bool ret = true;
    ret &= m_client->GetReceivedValue(key, value);
    return ret;
}

void BackendTest::GetServerParams()
{
    const char* ip_addr;
    const char* port_value;

    ip_addr = std::getenv("IP_ADDR");
    if (ip_addr!=NULL) {
        std::cout << "[BackendTest] The IP Address to connect is: " << ip_addr << std::endl;
    }
    else {
        std::cout << "[BackendTest] " << "Please set the \"IP_ADDR\"" << std::endl;
    }

    port_value = std::getenv("PORT");
    if (port_value!=NULL) {
        std::cout << "[BackendTest] The Port to connect is: " << port_value << std::endl;
    }
    else {
        std::cout << "[BackendTest] " << "Please set the \"PORT\"" << std::endl;
    }

    ip = ip_addr;
    port = std::stoi(port_value);
}
