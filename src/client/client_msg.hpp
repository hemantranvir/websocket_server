#ifndef CLIENT_MSG_HPP__
#define CLIENT_MSG_HPP__

#include <string>

enum Condition
{
    kRecv = 1,     //Expects string as argument to compare against received message
    kDelay = 2,    //Wait in milliseconds
};

struct ClientMsg
{
public:
    ClientMsg();
    ClientMsg(const std::string& msg_, const Condition condn_, const std::string& arg_);
    ClientMsg(const std::string& msg_, const Condition condn_, const int arg_);

    std::string msg;
    Condition condn;
    std::string arg_s;
    int arg_i;
};

#endif // CLIENT_MSG_HPP__
