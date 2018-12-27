#include "request_handler_factory.hpp"

RequestHandlerFactory::RequestHandlerFactory(std::ostream& os)
    : m_req_queue()
    , m_conn_id(0)
    , m_err_stream(os)
{
}

RequestHandlerFactory::~RequestHandlerFactory()
{
    Finalize();
}

void RequestHandlerFactory::Finalize(void)
{
    m_req_queue.Exit();
}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    m_err_stream << "[RequestHandlerFactory] " << "Request from " << request.clientAddress().toString() << std::endl
              << ": " << request.getMethod() << std::endl
              << ": " << request.getURI() << std::endl
              << ": " << request.getVersion() << std::endl;

    for (Poco::Net::HTTPServerRequest::ConstIterator it = request.begin(); it != request.end(); ++it)
    {
        m_err_stream << it->first + ": " + it->second << std::endl;
    }

    //Removes no longer used threads from threadpool
    Poco::ThreadPool::defaultPool().collect();

    m_err_stream << "[RequestHandlerFactory] " << " Threads available are: " << Poco::ThreadPool::defaultPool().available() << std::endl;

    if(request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0 && Poco::ThreadPool::defaultPool().available() > 0)
    {
        m_err_stream << "[RequestHandlerFactory] " << "[" << m_conn_id << "] " << "Creating WebSocketConnection Instance" << std::endl;
        WebSocketConnection* connection = new WebSocketConnection(m_conn_id, &m_req_queue, m_err_stream);
        m_err_stream << "[RequestHandlerFactory] " << "[" << m_conn_id << "] " << "WebSocketConnection Instance Created" << std::endl;
        m_conn_id++;
        return connection;
    }
    else if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0 && Poco::ThreadPool::defaultPool().available() == 0)
    {
        m_err_stream << "[RequestHandlerFactory] " << " Threads Not available" << std::endl;
        return nullptr;
    }
    else
    {
        m_err_stream << "[RequestHandlerFactory] " << " Page Request received" << std::endl;
        return new PageConnection();
    }
}
