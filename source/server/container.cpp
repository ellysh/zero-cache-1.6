#include "container.h"

#include <algorithm>
#include <string.h>

#include "functions.h"

using namespace std;
using namespace zero_cache;

static void RemoveMsg(Container::DataMap::value_type msg_pair)
{
    zmq_msg_close(&msg_pair.second);
}

Container::~Container()
{
    for_each(map_.begin(), map_.end(),
             RemoveMsg);
}

void Container::WriteData(const string& key, const zmq_msg_t& data)
{
    if ( map_.count(key) != 0 )
        zmq_msg_copy(&map_[key], const_cast<zmq_msg_t*>(&data));
    else
    {
        size_t size = ZmqMsgSize(data);

        zmq_msg_t msg;
        zmq_msg_init_size(&msg, size);
        memcpy(ZmqMsgData(msg), ZmqMsgData(data), size);

        map_.insert(DataMap::value_type(key, msg));
    }
}

zmq_msg_t* Container::ReadData(const string& key) const
{
    if ( map_.count(key) != 0 )
        return &map_[key];
    else
        return NULL;
}
