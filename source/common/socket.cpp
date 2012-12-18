#include "socket.h"

#include "zsignal.h"
#include "functions.h"
#include "connection.h"

using namespace std;
using namespace zero_cache;

Socket::Socket(SocketType type) : msg_(NULL)
{
    context_ = zctx_new();

    if ( type == kDealer )
    {
        in_socket_ = zsocket_new(context_, ZMQ_DEALER);
        out_socket_ = zsocket_new(context_, ZMQ_DEALER);
    }
    else if (type == kPubSub)
    {
        in_socket_ = zsocket_new(context_, ZMQ_SUB);
        /* FIXME: Is this subscription to all incoming messagees correct? */
        zmq_setsockopt(in_socket_, ZMQ_SUBSCRIBE, "", 0);
        out_socket_ = zsocket_new(context_, ZMQ_PUB);
    }

    zmq_pollitem_t items[1] = { { in_socket_, 0, ZMQ_POLLIN, 0 } };
    memcpy(&items_, &items, sizeof(items));
}

Socket::~Socket()
{
    zsocket_destroy(context_, out_socket_);
    zsocket_destroy(context_, in_socket_);
    zctx_destroy(&context_);
}

static Connection IncrementPort(Connection& connection)
{
    Connection result(connection);
    int port = connection.GetPort() + 1;
    result.SetPort(port);

    return result;
}

void Socket::Connect(Connection& connection)
{
    zsocket_connect(out_socket_, connection.GetString().c_str());

    Connection in_connection = IncrementPort(connection);

    zsocket_connect(in_socket_, in_connection.GetString().c_str());
}

void Socket::Bind(Connection& connection)
{
    zsocket_bind(in_socket_, connection.GetString().c_str());
    SetPermission(connection.GetString());

    Connection out_connection = IncrementPort(connection);

    zsocket_bind(out_socket_, out_connection.GetString().c_str());
    SetPermission(out_connection.GetString());
}

bool Socket::ReceiveMsg(long timeout)
{
    if ( msg_ != NULL )
        zmsg_destroy(&msg_);

    int result = zmq_poll(items_, 1, timeout);

    if ( result == 0 )
        return false;

    if (result == -1)
    {
        if ( zmq_errno() == ERR_INTERRUPT )
            exit(0);
    }

    if ( ! (items_[0].revents & ZMQ_POLLIN) )
        return false;

    msg_ = zmsg_recv(in_socket_);
    assert( msg_ != NULL );
    return true;
}

zframe_t* Socket::PopFrame()
{
    return zmsg_pop(msg_);
}

void Socket::SendFrame(zframe_t* frame, int flags)
{
    zframe_send(&frame, out_socket_, flags);
}

void Socket::SetQueueSize(int size)
{
    zsocket_set_hwm(in_socket_, size);
    zsocket_set_hwm(out_socket_, size);
}
