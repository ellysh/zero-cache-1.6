#include "registrar_client.h"

#include "client.h"
#include "functions.h"

#define STREAM Log()
#include "speed_test.h"

using namespace std;
using namespace zero_cache;

static const long kReadAnswerTimeout = 10;

RegistrarClient::RegistrarClient(string log_file, Connection connection, SocketType type) :
    Debug(log_file), socket_(type), clients_(connection, type)
{
    srand(time(NULL));

    socket_.Connect(connection);
    socket_.SetQueueSize(1);
}

void RegistrarClient::WriteData(string key, void* data, size_t size)
{
    Log() << "RegistrarClient::WriteData() - key = " << key << " data_size = " << size << endl;

    PRE_TIME_MEASURE("RegistrarClient::WriteData() ")

    GetClient(key)->WriteData(key, data, size);

    POST_TIME_MEASURE
}

void* RegistrarClient::ReadData(string key)
{
    Log() << "RegistrarClient::ReadData() - key = " << key << endl;

    PRE_TIME_MEASURE("RegistrarClient::ReadData() ")

    void* result = GetClient(key)->ReadData(key);

    POST_TIME_MEASURE

    return result;
}

Client* RegistrarClient::GetClient(string key)
{
    AddKey(key);

    return clients_.GetClient(key);
}

void RegistrarClient::AddKey(string key)
{
    if ( clients_.IsKeyExist(key) )
        return;

    int port = ReceivePort(key);

    Log() << "RegistrarClient::AddKey() - add key = " << key << " port = " << port << endl;

    clients_.AddKey(key, port);

    clients_.CreateClient(key, port);
}

int RegistrarClient::ReceivePort(string key)
{
    int port = kErrorPort;
    zframe_t* key_frame = zframe_new(key.c_str(), key.size());

    while ( port == kErrorPort )
    {
        socket_.SendFrame(key_frame, ZFRAME_REUSE);
        port = ReceiveAnswer(key_frame);

        usleep((rand() % 1000) * 1000);
    }

    zframe_destroy(&key_frame);

    return port;
}

int RegistrarClient::ReceiveAnswer(zframe_t* key)
{
    if ( ! socket_.ReceiveMsg(kReadAnswerTimeout) )
        return kErrorPort;

    zframe_t* key_frame = socket_.PopFrame();

    if ( ! zframe_eq(key_frame, key) )
    {
        zframe_destroy(&key_frame);
        return kErrorPort;
    }

    zframe_t* connection_frame = socket_.PopFrame();
    int port = FrameToInt(connection_frame);

    zframe_destroy(&key_frame);
    zframe_destroy(&connection_frame);

    return port;
}

void RegistrarClient::SetQueueSize(int size)
{
    clients_.SetQueueSize(size);
}
