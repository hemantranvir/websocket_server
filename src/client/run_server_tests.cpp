#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    char* ip;
    ip = std::getenv("IP_ADDR");
    if (ip!=NULL) {
        std::cout << "The IP Address to connect is: " << ip << std::endl;
    }
    else {
        std::cout << "Please set the \"IP_ADDR\"" << std::endl;
        return 0;
    }

    char* port;
    port = std::getenv("PORT");
    if (port!=NULL) {
        std::cout << "The Port to connect on is: " << port << std::endl;
    }
    else {
        std::cout << "Please set the \"PORT\"" << std::endl;
        return 0;
    }

    std::cout << std::flush;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
