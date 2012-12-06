#ifndef KEY_LIST_H
#define KEY_LIST_H

#include <string>
#include <map>

#include "types_zcache.h"

namespace zero_cache
{

class KeyList
{
private:
    typedef std::map<std::string, std::string> KeyPort;

public:
    KeyList(std::string connection);
    ~KeyList();

    void AddKey(std::string key);
    std::string GetConnection(std::string key);

private:
    int current_port_;
    std::string host_;
    KeyPort ports_;

    DISALLOW_COPY_AND_ASSIGN(KeyList)
};

}

#endif