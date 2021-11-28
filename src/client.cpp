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