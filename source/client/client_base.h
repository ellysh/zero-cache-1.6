#ifndef CLIENT_BASE_H
#define CLIENT_BASE_H

#include <string>
#include <zmq.h>

#include "debug.h"
#include "types_zcache.h"
#include "socket.h"
#include "answer.h"

namespace zero_cache
{

class Connection;
class Request;

class ClientBase : protected Debug
{
public:
    ClientBase(const char* log_file, Connection connection, const SocketType type);
    virtual ~ClientBase();

    virtual void SetQueueSize(const int size) = 0;

    virtual void SetHost(const std::string host);

protected:
    Socket socket_;
    Request* request_;
    port_t id_;
    Answer answer_;

    zmq_msg_t* SendRequest();
    zmq_msg_t* ReceiveAnswer();

private:
    DISALLOW_COPY_AND_ASSIGN(ClientBase);
};

}

#endif
