#include "client_msg.hpp"
#include <sstream>
#include <iomanip>

ClientMsg::ClientMsg()
    : msg("")
    , condn(kRecv)
    , arg_s("")
    , arg_i(0)
{
}

ClientMsg::ClientMsg(const std::string& msg_, const Condition condn_, const std::string& arg_)
    : msg(msg_)
    , condn(condn_)
    , arg_s(arg_)
    , arg_i(0)
{
}

ClientMsg::ClientMsg(const std::string& msg_, const Condition condn_, const int arg_)
    : msg(msg_)
    , condn(condn_)
    , arg_s("")
    , arg_i(arg_)
{
}
