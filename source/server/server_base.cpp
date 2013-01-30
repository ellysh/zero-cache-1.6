#include "server_base.h"

#include <assert.h>

#include "interrupt_signal.h"
#include "functions.h"
#include "connection.h"

using namespace std;
using namespace zero_cache;

ServerBase::ServerBase(const char* log_file, Connection connection, SocketType type) :
    Debug(log_file), socket_(type)
{
    socket_.BindIn(connection);
    socket_.SetQueueSize(1000);
}

void ServerBase::Start()
{
    s_catch_signals();

    while (! s_interrupted )
        ProcessMessage();
}
