#ifndef _WEBSOCKET_SERVER_HPP__
#define _WEBSOCKET_SERVER_HPP__

#include <fstream>
#include <iostream>
#include <string>
#include "request_handler_factory.hpp"

class WebSocketServer: public Poco::Util::ServerApplication
{
public:
    WebSocketServer();
    explicit WebSocketServer(std::ostream& os);
    explicit WebSocketServer(int port);
    WebSocketServer(int port, std::ostream& os);
    ~WebSocketServer();
    bool SetMaxThreads(int max_threads);
    bool Initialize();
    bool Run();
    bool StopAll(bool force_stop);

protected:
    bool  main();

private:
    std::unique_ptr<Poco::Net::HTTPServer> m_server;
    int m_port;
    std::ofstream m_null_stream;
    std::ostream& m_err_stream;
    int m_max_threads;
    std::mutex m_mutex;
    std::condition_variable m_condn;
    bool m_server_running;
};

#endif //_WEBSOCKET_SERVER_HPP__
