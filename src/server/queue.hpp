#ifndef _QUEUE_HPP__
#define _QUEUE_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Queue
{
public:
    Queue();
    Queue(std::ostream& os);
    ~Queue();

    void Enqueue(const std::pair<std::string, std::string>& req, std::string caller);
    std::pair<std::string, std::string> Dequeue(std::string caller);
    bool Empty();
    void Exit();

private:
    std::queue<std::pair<std::string, std::string>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond; 
    bool m_exit;
    std::ofstream m_null_stream;
    std::ostream& m_err_stream;
};

#endif //_QUEUE_HPP__
