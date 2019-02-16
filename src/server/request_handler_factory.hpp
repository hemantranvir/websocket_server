#ifndef _REQUEST_HANDLER_FACTORY_HPP__
#define _REQUEST_HANDLER_FACTORY_HPP__

#include <iostream>
#include <string>
#include "websocket_connection.hpp"
#include "page_connection.hpp"

class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
    RequestHandlerFactory(std::ostream& os);
    ~RequestHandlerFactory(void);

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
    RequestHandlerFactory(void);
    void Finalize(void);

    Queue m_req_queue;
    std::ofstream m_null_stream;
    std::ostream& m_err_stream;
};

#endif //_REQUEST_HANDLER_FACTORY_HPP__
