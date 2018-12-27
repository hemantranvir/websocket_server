#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "websocket_server.hpp"

int main(int argc, char** argv)
{
    int pos = 1;
    int port = 32000;
    while (pos < argc) {
        std::string cur_arg(argv[pos]);
        if (std::string(argv[pos]) == "-i") {
            try {
                port = std::stoi(argv[++pos]);
            }
            catch (...) {
                std::cout << "Error: Invalid port value" << std::endl;
                return -1;
            }
	} else {
            std::cout << "Error: unknown option: " << argv[pos] << std::endl;
            return -1;
        }
        ++pos;
    }

    WebSocketServer server_app(port, std::cout);
    server_app.SetMaxThreads(64);
    if (!server_app.Initialize()) {
        std::cout << "Error: Failed to initialize server." << std::endl;
        return -1;
    }
    server_app.Run();

    std::string input;
    do
    {
        std::cout << "To Terminate Server Operation Enter \"exit\"" << std::endl;
        std::cin >> input;
    } while (input != "exit");

    std::cout << "...exiting" << std::endl;

    //server_app.StopAll(true);
}
