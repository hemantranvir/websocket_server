#include "queue.hpp"

Queue::Queue(void) 
    : m_queue() 
    , m_mutex() 
    , m_cond()
    , m_exit(false)
    , m_err_stream(m_null_stream)
{
    m_err_stream << "[Queue]" << " Queue Ctor called" << std::endl;
}

Queue::Queue(std::ostream& os) 
    : m_queue() 
    , m_mutex() 
    , m_cond()
    , m_exit(false)
    , m_err_stream(os)
{
    m_err_stream << "[Queue]" << " Queue Ctor called" << std::endl;
}

Queue::~Queue(void) 
{
    m_err_stream << "[Queue]" << " Queue Dtor called" << std::endl;
} 

void Queue::Enqueue(const std::pair<int, std::string>& req, std::string caller)
{
#ifdef DEBUG
    m_err_stream << "[Queue]" << "[Caller: " << caller << "]" << " Enqueue Called, Req Value: " << req.second << std::endl;
#endif
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(req);
    m_cond.notify_all();
#ifdef DEBUG
    m_err_stream << "[Queue]" << "[Caller: " << caller << "]" << " Enqueue Finished" << std::endl;
#endif
}

std::pair<int, std::string> Queue::Dequeue(std::string caller)
{
#ifdef DEBUG
    m_err_stream << "[Queue]" << "[Caller: " << caller << "]" << " Dequeue Called" << std::endl;
#endif
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_queue.empty() && !m_exit)
    {
#ifdef DEBUG
        m_err_stream << "[Queue]" << "[Caller: " << caller << "]" << " Waiting for Notification from Enqueue" << std::endl;
#endif
        m_cond.wait(lock, [this]() { return !m_queue.empty() || m_exit; });
#ifdef DEBUG
        m_err_stream << "[Queue]" << "[Caller: " << caller << "]" << " Dequeue Wait Finish" << std::endl;
#endif
    }

    if (m_queue.empty() && m_exit) return std::pair<int, std::string> (-1, "");

    std::pair<int, std::string> val = m_queue.front();
#ifdef DEBUG
    m_err_stream << "[Queue]" << "[Caller: " << caller << "]"<< " Deque value: " << val.second << std::endl;
#endif
    m_queue.pop(); 
    return val; 
} 

bool Queue::Empty()
{
    return m_queue.empty();
}

void Queue::Exit()
{
    std::unique_lock<std::mutex> lock(m_mutex); 
    m_exit = true;
    m_cond.notify_all();
}
