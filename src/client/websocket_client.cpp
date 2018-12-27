#include "websocket_client.hpp"

template <typename Var>
bool var_compare (Var const &lhs, Var const &rhs) {
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
}

WebSocketClient::WebSocketClient(const std::string& ip, int port, std::ostream& os)
    : m_finalizing(false)
    , m_ip(ip)
    , m_port(port)
    , m_client_status(true)
    , m_ws(nullptr)
    , m_err_stream(os)
{
    Connect();
}

WebSocketClient::WebSocketClient(const std::string& ip, int port)
    : m_finalizing(false)
    , m_ip(ip)
    , m_port(port)
    , m_client_status(true)
    , m_ws(nullptr)
    , m_err_stream(m_null_stream)
{
    Connect();
}

WebSocketClient::~WebSocketClient()
{
    Join();
}

void WebSocketClient::Join()
{
    m_err_stream << "[Client] " << "Join Called" << std::endl;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_ws)
        {
            m_ws->shutdown();
            delete m_ws;
            m_ws = nullptr;
        }
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_finalizing)
        {
            m_finalizing = true;
            m_condn.notify_all();
        }
    }

    m_err_stream << "[Client] " << "Join Finished" << std::endl;
}

bool WebSocketClient::operator()()
{            
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_client_status;
}

bool WebSocketClient::SendAndCheckCommand(const ClientMsg& msg)
{
    bool ret = true;
    std::unique_lock<std::mutex> lock(m_mutex);
    //m_err_stream << "[Client] " << "SendAndCheckCommand Called" << std::endl;

    if (!m_ws) {
        return false;
    }

    //char receive_buff[4096*2] = {'\0'};
    const int frame_size = 1<<14; 
    std::vector<char> receive_buff(frame_size, '\0');
    int len, flags;

    try 
    {
        if (msg.condn == kDelay)
        {
            m_err_stream << "[Client] " << "[Client] kDelay Message: waiting.." << std::endl;
            usleep(msg.arg_i * 1000);
        }
        else {
            m_err_stream << "[Client] " << "Message to send: " << msg.msg << std::endl;
            //strncpy(buffer, msg.msg.c_str(), 1024);
            //m_ws->sendFrame(buffer, strlen(buffer), Poco::Net::WebSocket::FRAME_TEXT);
            m_ws->sendFrame(msg.msg.c_str(), strlen(msg.msg.c_str()), Poco::Net::WebSocket::FRAME_TEXT);

            len = m_ws->receiveFrame(&receive_buff[0], frame_size, flags);
            //len = m_ws->receiveFrame(receive_buff, 4096*2 , flags);

            if (len > 0)
            {
                //m_err_stream << "[Client] " << "Received Msg: " << receive_buff << std::endl;
                m_err_stream << "[Client] " << "Received Msg: " << receive_buff.data() << std::endl;
                m_err_stream << "[Client] " << "Saved Msg: " << msg.arg_s << std::endl;
                m_recv_msg = std::string(receive_buff.data());
                //m_recv_msg = receive_buff;
                m_saved_msg = msg;

                if (msg.condn == kRecv)
                {
                    if (!CheckMsg(&receive_buff[0])) {
                        m_err_stream << "[Client] " << "\033[1;31m" << "[Error] CheckMsg Failed!!" << "\033[0m" << std::endl;
                        m_client_status = false;
                        ret &= false;
                    }
                    else {
                        m_err_stream << "[Client] " << "CheckMsg OK!!" << std::endl;
                    }
                }
            }
        }

        //std::memset(receive_buff, '\0', 1024);
        receive_buff.clear();
    }

    catch (Poco::Exception& e)
    {
        m_err_stream << "[Client] " << " Poco::Exception " << e.what() << std::endl;
        std::unique_lock<std::mutex> lock(m_mutex);
        m_client_status = false;
        ret &= false;
    }
    //m_err_stream << "[Client] " << "SendAndCheckCommand Finished" << std::endl;

    return ret;
}

bool WebSocketClient::GetReceivedValue(const std::string& key, std::string& value)
{
    bool ret = true;
    std::unique_lock<std::mutex> lock(m_mutex);
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var recv, recv_data;
    Poco::JSON::Object::Ptr recv_obj, recv_obj_data;
    Poco::JSON::Array::Ptr arr_recv;
    Poco::JSON::Array::ValueVec::const_iterator it_arr_recv;

    std::string key_ = boost::algorithm::erase_all_copy(key, "\"");

    const std::string msg(m_recv_msg);
    m_err_stream << "[Client] [GetReceivedValue] " << "Recv Msg: " << msg << std::endl;
    //Parse the received message
    try
    {
        recv = parser.parse(msg);
        recv_obj = recv.extract<Poco::JSON::Object::Ptr>();
    }
    catch (Poco::Exception& e)
    {
        m_err_stream << "[Client] [GetReceivedValue] " << "Recv Msg: " << msg << std::endl;
        m_err_stream << "[Client] [GetReceivedValue] Poco::Exception failed to parse the recv msg " << e.what() << ", Display Text: " << e.displayText() << std::endl;
        ret &= false;
        return ret;
    }

    if (recv_obj->has("data")) {
        recv_data = recv_obj->get("data");
    }
    else {
        m_err_stream << "[Client] [GetReceivedValue] " << "Key \"data\" not found in received message"  << std::endl;
        ret &= false;
        return ret;
    } 

    if (recv_data.isArray())
    {
        arr_recv = recv_data.extract<Poco::JSON::Array::Ptr>();
    }
    else
    {
        m_err_stream << "[Client] [GetReceivedValue] " << "Received Message's Key \"data\" is not array" << std::endl;
        ret &= false;
        return ret;
    }

    for (it_arr_recv = arr_recv->begin(); it_arr_recv != arr_recv->end(); ++it_arr_recv)
    {
        recv_obj_data = it_arr_recv->extract<Poco::JSON::Object::Ptr>();

        if (recv_obj_data->has(key_)) {
            value = recv_obj_data->getValue<std::string>(key_);
            break;
        }
        else {
            m_err_stream << "[Client] [GetReceivedValue] " << "Key \"" << key_ << "\" not found in received message"  << std::endl;
            ret &= false;
        }
    }

    return ret;
}

void WebSocketClient::Connect(void)
{
    m_err_stream << "[Client] " << "Starting WebSocketClient.." << std::endl;
    std::unique_lock<std::mutex> lock(m_mutex);
    Poco::Net::HTTPClientSession cs(m_ip, m_port);
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/?encoding=text",Poco::Net::HTTPMessage::HTTP_1_1);
    request.set("origin", "http://localhost");
    Poco::Net::HTTPResponse response;
    Poco::JSON::Object::Ptr object;

    try 
    {
        m_err_stream << "[Client] " << "Creating Socket.." << std::endl;
        m_ws = new Poco::Net::WebSocket(cs, request, response);
        m_err_stream << "[Client] " << "Socket created.." << std::endl;
    }

    catch (Poco::Exception& e)
    {
        m_err_stream << "[Client] " << " Poco::Exception " << e.what() << std::endl;
        std::unique_lock<std::mutex> lock(m_mutex);
        m_client_status = false;
    }

    catch (std::exception &e) 
    {
        m_err_stream << "[Client] " << "Exception " << e.what();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_client_status = false;
    }
}

bool WebSocketClient::CheckMsg(const char* recv_buff)
{
    //m_err_stream << "[Client] " << "CheckMsg Called " << std::endl;
    bool ret = true;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var recv_result, saved_result;
    Poco::JSON::Object::Ptr recv_obj, saved_obj;

    const std::string& recv_msg(recv_buff);
    const std::string& saved_msg(m_saved_msg.arg_s);

    //Parse the saved message
    try
    {
        saved_result = parser.parse(saved_msg);
        saved_obj = saved_result.extract<Poco::JSON::Object::Ptr>();
    }
    catch (Poco::Exception& e)
    {
        m_err_stream << "[Client] " << "Saved Msg: " << saved_msg << std::endl;
        m_err_stream << "[Client] Poco::Exception failed to parse the saved msg " << e.what()  << ", Display Text: " << e.displayText() << std::endl;
        return false;
    }

    //Parse the received message
    try
    {
        recv_result = parser.parse(recv_msg);
        recv_obj = recv_result.extract<Poco::JSON::Object::Ptr>();
    }
    catch (Poco::Exception& e)
    {
        m_err_stream << "[Client] " << "Recv Msg: " << recv_msg << std::endl;
        m_err_stream << "[Client] Poco::Exception failed to parse the recv msg " << e.what() << ", Display Text: " << e.displayText() << std::endl;
        return false;
    }

    if (recv_msg == saved_msg) {
        std::cout << "[Client] " << "CheckMsg OK!" << std::endl;
    } else {
        std::cout << "[Client] " << "CheckMsg Failed!" << std::endl;
        ret &= false;
    }

    return ret;
}
