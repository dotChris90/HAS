#include "utils.hpp"


std::string FromUA2String(UA_String ua_string)
{
    char* convert = (char*)UA_malloc(sizeof(char)*ua_string.length+1);
    memcpy(convert, ua_string.data, ua_string.length );
    convert[ua_string.length] = '\0';
    std::string local_string = std::string(convert);
    delete convert;
    return local_string;
}