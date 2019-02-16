#include "websocket_connection.hpp"

WebSocketConnection::WebSocketConnection(std::string conn_id, Queue* req_queue)
    : m_conn_id(conn_id)
    , m_req_queue(req_queue)
    , m_res_queue()
    , m_finalizing(false)
    , m_err_stream(m_null_stream)
    {
        m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Registering Response Queue to ctrl manager" << std::endl;
    }

WebSocketConnection::WebSocketConnection(std::string conn_id, Queue* req_queue, std::ostream& os)
    : m_conn_id(conn_id)
    , m_req_queue(req_queue)
    , m_res_queue(os)
    , m_finalizing(false)
    , m_err_stream(os)
    {
        m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Registering Response Queue to ctrl manager" << std::endl;
    }

WebSocketConnection::~WebSocketConnection()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_finalizing = true;
    }

    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Unregistering Response Queue from ctrl manager" << std::endl;
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Calling Response Queue Exit" << std::endl;
    m_res_queue.Exit();
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Thread Join Called" << std::endl;
    m_reply.join();
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "ReplyCore Join Finished" << std::endl;
}

void WebSocketConnection::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Request Received" << std::endl;
    Poco::Net::WebSocket ws(request, response);
    ws.setReceiveTimeout(Poco::Timespan(10, 0, 0, 0, 0)); //Receive Timeout of 10 days
#ifdef DEBUG
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "WebSocket connection established!" << std::endl;
#endif

    m_reply = std::thread(std::bind(&ReplyFunc, this, ws));
    int flags = 0, n = 0;
    do {
        try {
            const int frame_size = 1 << 14;
            std::vector<char> buffer(frame_size, '\0');
#ifdef DEBUG
            m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Waiting for incoming frame" << std::endl;
#endif
            n = ws.receiveFrame(buffer.data(), frame_size, flags);

#ifdef DEBUG
            m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Received Msg: " << buffer.data() << ", Received Bytes: " << n << std::endl;
#endif
            if (n > 0) {
#ifdef DEBUG
                m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Frame received" << std::endl;
#endif
                const std::string& json(buffer.data());
                m_req_queue->Enqueue(std::pair<std::string, std::string> (m_conn_id, json), "ReceiveThread");
            }
        } catch (Poco::Net::WebSocketException& e) {
            switch (e.code()) {
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
            case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
                response.setContentLength(0);
                response.send();
                break;
            }
        } catch (Poco::Exception& e) {
            m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Poco::Exception " << e.what() << std::endl;
            break;
        } catch (std::exception& e) {
            m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Exception " << e.what() << std::endl;
            break;
        }
    } while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);

    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "WebsocketConnection Closed" << std::endl;
}

void WebSocketConnection::ReplyFunc(void* obj, Poco::Net::WebSocket& ws)
{
    WebSocketConnection* ws_handler = reinterpret_cast<WebSocketConnection*>(obj);
    ws_handler->ReplyCore(ws);
}

void WebSocketConnection::ReplyCore(Poco::Net::WebSocket& ws)
{
    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "ReplyCore Called" << std::endl;

    do
    {
        try {
        //const std::string& res(m_res_queue.Dequeue("ReplyCore").second);

        const std::string res("{\"command\":\"INITID\",\"status\":\"OK\"}");

        int res_bytes = res.size();
        const char *res_ptr = &res[0];

#ifdef DEBUG
        m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Replying to client" << std::endl;
#endif
        if (!res.empty())
        {
            do
            {
                int sent_bytes = ws.sendFrame(res_ptr, res_bytes, Poco::Net::WebSocket::FRAME_TEXT);

                if (sent_bytes != res_bytes)
                {
                    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Sent Bytes not equal to buffer length" << std::endl;
                }

                res_bytes -= sent_bytes;
                res_ptr += sent_bytes;
            } while (res_bytes > 0);
        }
#ifdef DEBUG
        m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "Replying to client finished" << std::endl;
#endif

        }

        catch (...) {
            m_err_stream << "[WebSocketConnection] Caught an exception in ReplyCore" << std::endl;
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_finalizing) break;
        }

    } while (true);

    m_err_stream << "[WebSocketConnection] " << "[" << m_conn_id << "] " << "ReplyCore Thread Exiting.." << std::endl;
}
