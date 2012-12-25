#include "functions.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;
using namespace zero_cache;

static const size_t kFileNamePos = 6;

port_t zero_cache::FrameToPort(zframe_t* frame)
{
    int* result = (int*)zframe_data(frame);

    return *result;
}

std::string zero_cache::FrameToString(zframe_t* frame)
{
    char* buffer =  zframe_strdup(frame);
    string result = buffer;
    free(buffer);

    return result;
}

port_t zero_cache::StringToPort(string& value)
{
    char* endptr;
    port_t result = strtoul(value.c_str(), &endptr, 10);

    if ( endptr != value.c_str() )
        return result;
    else
        return kErrorPort;
}

void zero_cache::SetPermission(const char* connection)
{
    if ( strlen(connection) <= kFileNamePos )
        return;

    const char* file = connection + kFileNamePos;
    chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
}

port_t zero_cache::GenerateId(void* pointer)
{
    port_t result = (port_t)pointer;
    return result;
}
