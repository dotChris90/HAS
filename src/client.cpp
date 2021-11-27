#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <string>

#include "interface.h"
#include "serial.hpp"
#include "DiscoveryServer.hpp"

#include <stdlib.h>
#include <thread>
#include <iostream>
#include <thread>
#include "FileWatch.hpp"

#include <map>

using namespace std::chrono_literals;

UA_Boolean running = true;

/*
static std::map<std::string,my_server_status> *servers;
static void* adresse;


bool started_server = false;

static filewatch::FileWatch<std::string>* watch;


void on_delete(const std::string& path, const filewatch::Event change_type);

void on_delete(const std::string& path, const filewatch::Event change_type)
{
    std::string deleted_msg = "Server --> " + path + " stopped .... \n";
    printf(deleted_msg.c_str());
    (*servers).erase(path);
}

void on_register(const UA_RegisteredServer *registeredServer, void* data)
{
    std::string semaphore = FromUA2String(registeredServer->semaphoreFilePath);   
    std::string connected_msg = "Server " + semaphore + " connected \n";
    printf(connected_msg.c_str());
    UA_String a = registeredServer->serverUri;
    watch = new filewatch::FileWatch<std::string>(semaphore,on_delete);
    my_server_status state;
    state.is_on = true;
    state.url = FromUA2String(registeredServer->serverUri);
    (*servers)[semaphore] = state;
}

void start_server(UA_Server *server)
{
    UA_ServerConfig *config = UA_Server_getConfig(server);
    UA_ServerConfig_setMinimal(config,4444,nullptr);

    
    config->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;

    UA_String_clear(&config->applicationDescription.applicationUri);
    config->applicationDescription.applicationUri =
            UA_String_fromChars("urn:open62541.example.local_discovery_server");
    started_server = true;
    
    UA_StatusCode retval1 = UA_Server_run(server, &running);
}

*/

int main(void)
{

    HAS::DiscoveryServer discovery;
    discovery.PrepareListeningOn(4444);
    discovery.StartRunning();

    std::this_thread::sleep_for(5000ms);

    //UA_Client *client = UA_Client_new();
    //UA_Client *client2 = UA_Client_new();
    //UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    //UA_ClientConfig_setDefault(UA_Client_getConfig(client2));

    //UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4322");

    //if(retval != UA_STATUSCODE_GOOD) {
    //    UA_Client_delete(client);
    //    return (int)retval;
    //}

    //retval = UA_Client_connect(client2, "opc.tcp://localhost:4321");
    //if(retval != UA_STATUSCODE_GOOD) {
    //    UA_Client_delete(client);
    //    return (int)retval;
    //}

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
    /*const UA_NodeId node_obj = UA_NODEID_NUMERIC(1,62540);
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

    std::string topic_ = FromUA2String(topic);
    
    if_name = UA_STRING("LOC");
    UA_Variant_setScalarCopy(&input[1], &if_name, &UA_TYPES[UA_TYPES_STRING]);
    retval = UA_Client_call(client2, node_obj, node_meth, 2, input, &outputSize, &output);
    UA_String topic_2 =  *(UA_String *) output[0].data;
    UA_Int64 size_struct_2 = *(UA_Int64 *) output[1].data;

    // ToDo : find better way for UA_STRING --> std::string -.- 
    std::string topic_2_ = FromUA2String(topic_2);
    
    */

    while (true)
    {
        for(auto const& server : discovery.KnownServers)
        {
            auto server_connector = server.second;
            if (server_connector.HasTopics() && server_connector.online )
            {
                std::string topic;
                int topic_size;
                std::tie(topic,topic_size) = server_connector.GetTopics()[0];
                int fd = shm_open(topic.c_str(), O_RDWR , S_IRUSR | S_IWUSR);
                int res = ftruncate(fd,topic_size);
                void* addr = mmap(NULL,topic_size,PROT_WRITE,MAP_SHARED,fd,0);
                auto data = bla::serial::serialData(topic,addr);
                std::cout << "I check Interface behind shm_location " << topic << " ... but i dont know what structure is ";
                std::cout << data << std::endl;
            }
        }
        std::this_thread::sleep_for(2000ms);
    }
    

    //UA_Client_delete(client); /* Disconnects the client internally */
    return EXIT_SUCCESS;
}