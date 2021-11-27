#include "DiscoveryServer.hpp"
#include <thread>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>
#include "utils.hpp"

using namespace std::chrono_literals;

namespace HAS
{
    void on_registry(const UA_RegisteredServer *registeredServer, void* data)
    {
        DiscoveryServer* discovery = (DiscoveryServer*)data;
        ServerConnector new_server;
        std::string semaphore = FromUA2String(registeredServer->semaphoreFilePath);
        new_server.SetConnection(FromUA2String(registeredServer->serverUri));
        new_server.SetSemaphore(semaphore);
        discovery->KnownServers[semaphore] = new_server;
    }
    
    DiscoveryServer::DiscoveryServer()
    {

    }

    DiscoveryServer::~DiscoveryServer() 
    {

    }

    void DiscoveryServer::PrepareListeningOn(int port) 
    {
        this->_server = UA_Server_new();
        UA_ServerConfig *config = UA_Server_getConfig(this->_server);
        UA_ServerConfig_setMinimal(config,port,nullptr);
        config->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;
        UA_String_clear(&config->applicationDescription.applicationUri);
        config->applicationDescription.applicationUri = 
            UA_String_fromChars("urn:open62541.example.local_discovery_server");
    }
    
    void DiscoveryServer::StartRunning() 
    {
        std::thread server_thread(UA_Server_run,
            this->_server,
            &this->_is_running
        );
        server_thread.detach();
        std::this_thread::sleep_for(5000ms);
        UA_Server_setRegisterServerCallback(
            this->_server,
            [](const UA_RegisteredServer *registeredServer, void* data)
            {
                DiscoveryServer* discovery = (DiscoveryServer*)data;
                ServerConnector new_server;
                std::string semaphore = FromUA2String(registeredServer->semaphoreFilePath);
                new_server.SetConnection("opc.tcp://" + FromUA2String(registeredServer->serverUri));
                auto watcher = new filewatch::FileWatch<std::string>(
                    semaphore,
                    [&discovery](const std::string& path, const filewatch::Event change_type)
                    {
                        discovery->KnownServers.erase(path);
                    }   
                );
                new_server.Watch = watcher;
                new_server.SetSemaphore(semaphore);
                discovery->KnownServers[semaphore] = new_server;
                discovery->KnownServers[semaphore].Connect();
            },
            this
        );
    }
}


