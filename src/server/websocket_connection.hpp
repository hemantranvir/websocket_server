#ifndef _WEBSOCKET_REQUEST_HANDLER_HPP__
#define _WEBSOCKET_REQUEST_HANDLER_HPP__

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "queue.hpp"

class WebSocketConnection: public Poco::Net::HTTPRequestHandler
{
public:
    WebSocketConnection(std::string conn_id, Queue* req_queue);
    WebSocketConnection(std::string conn_id, Queue* req_queue, std::ostream& os);
    ~WebSocketConnection();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    static void ReplyFunc(void* obj, Poco::Net::WebSocket& ws);

private:
    void ReplyCore(Poco::Net::WebSocket& ws);

    std::string m_conn_id;
    Queue* m_req_queue;
    Queue m_res_queue;
    std::thread m_reply;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_finalizing;
    std::ofstream m_null_stream;
    std::ostream& m_err_stream;
};

#endif //_WEBSOCKET_REQUEST_HANDLER_HPP__
