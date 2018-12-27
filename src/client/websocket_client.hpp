#ifndef _WEBSOCKET_CLIENT_HPP__
#define _WEBSOCKET_CLIENT_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "boost/algorithm/string.hpp"
#include "client_msg.hpp"

class WebSocketClient
{
public:
    WebSocketClient(const std::string& ip, int port, std::ostream& os);
    WebSocketClient(const std::string& ip, int port);
    ~WebSocketClient();

    void Join(void);
    bool operator()();
    bool SendAndCheckCommand(const ClientMsg& msg);
    bool GetReceivedValue(const std::string& key, std::string& value);

private:
    void Connect(void);
    bool CheckMsg(const char* recv_buff);

    bool m_finalizing;
    std::string m_ip;
    int m_port;
    bool m_client_status;
    Poco::Net::WebSocket* m_ws;
    std::mutex m_mutex;
    std::condition_variable m_condn;
    std::ofstream m_null_stream;
    std::ostream& m_err_stream;
    std::string m_recv_msg;
    ClientMsg m_saved_msg;
};

#endif //_WEBSOCKET_CLIENT_HPP__
