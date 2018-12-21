#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "websocket_server.hpp"

int main(int argc, char** argv)
{
    int server_port = 32000;
    WebSocketServer server_app(server_port, std::cout);
    server_app.SetMaxThreads(64);
    if (!server_app.Initialize()) {
        std::cout << "Failed to initialize server." << std::endl;
        return -1;
    }
    server_app.Run();

    std::string input;
    do
    {
        std::cout << "To Terminate Server Operation Enter \"exit\"" << std::endl;
        std::cin >> input;
    } while (input != "exit");

    //server_app.StopAll(true);
}
