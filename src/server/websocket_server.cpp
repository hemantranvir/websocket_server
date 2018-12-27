#include "websocket_server.hpp"
#include "server_param.hpp"

WebSocketServer::WebSocketServer()
    : m_server(nullptr)
    , m_port(32000)
    , m_err_stream(m_null_stream)
    , m_max_threads(64)
    , m_server_running(false)
{
}

WebSocketServer::WebSocketServer(std::ostream& os)
    : m_server(nullptr)
    , m_port(32000)
    , m_err_stream(os)
    , m_max_threads(64)
    , m_server_running(false)
{
}

WebSocketServer::WebSocketServer(int port)
    : m_server(nullptr)
    , m_port(port)
    , m_err_stream(m_null_stream)
    , m_max_threads(64)
    , m_server_running(false)
{
}

WebSocketServer::WebSocketServer(int port, std::ostream& os)
    : m_server(nullptr)
    , m_port(port)
    , m_err_stream(os)
    , m_max_threads(64)
    , m_server_running(false)
{
}

WebSocketServer::~WebSocketServer()
{
    m_err_stream << "[WebSocketServer] " << "Websocket Dtor called" << std::endl;
    StopAll(true);
    m_err_stream << "[WebSocketServer] " << "Websocket Dtor Finished" << std::endl;
}

bool WebSocketServer::SetMaxThreads(int max_threads)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_max_threads = max_threads;
    return true;
}

bool WebSocketServer::Run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    //Starts the server. A new thread will be created from threadpool that waits for and accepts incoming connections.
    m_server->start();
    m_server_running = true;

    int threads_num = m_server->maxThreads();
    m_err_stream << "[WebSocketServer] " << "Maximum No of Threads/WebSocketConnections allowed are: " << threads_num << std::endl;

    std::cout << "--------WebSocketServer Created Succesfully--------" << std::endl;

    return true;
}

bool WebSocketServer::StopAll(bool force_stop)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_err_stream << "[WebSocketServer] " << "Stopping the Server by stopping every websocket connection..." << std::endl;
    //If force_stop is false, all current requests are allowed to complete
    //If force_stop is true, the underlying sockets of all client connections are shut down, causing all requests to abort
    if (m_server_running) {
        m_server->stopAll(force_stop);
    }

    //Wait for all threads in threadpool to finish
    Poco::ThreadPool::defaultPool().stopAll();
    Poco::ThreadPool::defaultPool().joinAll();
    return true;
}

bool WebSocketServer::Initialize()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_server != nullptr){
        m_err_stream << "[WebSocketServer]: Already initilaized" << std::endl;
    }
    //Poco::Net::HTTPServerParams* params = nullptr;
    ServerParam* params = nullptr;
    RequestHandlerFactory* handler_factory = nullptr;
    try {
        handler_factory = new RequestHandlerFactory(m_err_stream);

        //Setup the port
        unsigned short port = (unsigned short) config().getInt("WebSocketServer.port", m_port);
        m_err_stream << "[WebSocketServer]: Server port: " << port << std::endl;

        //Setup a server socket
        Poco::Net::ServerSocket svs(port);

        //Setup server parameters and threadpool parameters
        //params = new Poco::Net::HTTPServerParams;
        params = new ServerParam;
        m_err_stream << "[WebSocketServer] " << "Setting max threads as: " << m_max_threads << std::endl;
        params->setMaxThreads(m_max_threads);
        Poco::ThreadPool::defaultPool().addCapacity(m_max_threads - Poco::ThreadPool::defaultPool().available());

        //Setup the server
        m_err_stream << "[WebSocketServer] " << "Creating Server and Registering Handler Factory" << std::endl;
        m_server = std::unique_ptr<Poco::Net::HTTPServer>(new Poco::Net::HTTPServer(handler_factory, svs, params));

    } catch (const Poco::RuntimeException& e) {
        m_err_stream << "[WebSocketServer] Exception occurred at server initizlization." << std::endl;
        if (m_server == nullptr) {
            if (params) delete params;
            if (handler_factory) delete handler_factory;
        }
        return false;
    }
    return true;
}
