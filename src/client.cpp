/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

/**
 * Building a Simple Client
 * ------------------------
 * You should already have a basic server from the previous tutorials. open62541
 * provides both a server- and clientside API, so creating a client is as easy as
 * creating a server. Copy the following into a file `myClient.c`: */

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <string>

#include "interface.h"
#include "serial.hpp"

#include <stdlib.h>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

static void* adresse;

int main(void) {

    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }

    // section to try out converting between OPC-UA types and Std types
    // node_ids must be known 
    /*
    const UA_NodeId node_id = UA_NODEID_NUMERIC(0,53912);
    UA_QualifiedName ua_name;

    UA_Variant double_value;
    UA_Variant_init(&double_value);
    retval = UA_Client_readValueAttribute(client,node_id,&double_value);
    UA_Double qwer =  *(UA_Double *) double_value.data;
    double qwer_ = *(double*) double_value.data;
    double qwer__ = (double) qwer;
    void* p = double_value.data;
    */
    // method call trial#
    const UA_NodeId node_obj = UA_NODEID_NUMERIC(1,62540);
    const UA_NodeId node_meth = UA_NODEID_NUMERIC(1,62543);
    UA_QualifiedName ua_name;
    retval = UA_Client_readBrowseNameAttribute(client,node_meth,&ua_name);
    retval = UA_Client_readBrowseNameAttribute(client,node_obj,&ua_name);
    UA_Variant input[2];
    UA_String if_name = UA_STRING("CAN");
    UA_Int32 sample_rate = UA_Int32(1000);

    UA_Variant_init(input);
    UA_Variant_setScalarCopy(&input[0], &sample_rate, &UA_TYPES[UA_TYPES_INT32]);
    UA_Variant_setScalarCopy(&input[1], &if_name, &UA_TYPES[UA_TYPES_STRING]);
    
    size_t outputSize;
    UA_Variant *output;
    retval = UA_Client_call(client, node_obj, node_meth, 2, input, &outputSize, &output);
    
    UA_String topic =  *(UA_String *) output[0].data;
    UA_Int64 size_struct = *(UA_Int64 *) output[1].data;

    // ToDo : find better way for UA_STRING --> std::string -.- 
    char* convert = (char*)UA_malloc(sizeof(char)*topic.length+1);
    memcpy(convert, topic.data, topic.length );
    convert[topic.length] = '\0';
    
    std::string topic_ = std::string(convert);
    
    while (true)
    {
        int fd = shm_open(topic_.c_str(), O_RDWR , S_IRUSR | S_IWUSR);
        int res = ftruncate(fd,size_struct);
        void* addr = mmap(NULL,size_struct,PROT_WRITE,MAP_SHARED,fd,0);
        auto data = bla::serial::serialData(topic_,addr);
        std::cout << "I check Interface behind shm_location " << topic_ << " ... but i dont know what structure is ";
        std::cout << data << std::endl;
        std::this_thread::sleep_for(2000ms);
    }

    UA_Client_delete(client); /* Disconnects the client internally */
    return EXIT_SUCCESS;
}