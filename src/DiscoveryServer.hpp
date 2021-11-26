#pragma once 

#include "ServerConnector.hpp"
#include <map>
#include <open62541/server.h>

namespace HAS {
    class DiscoveryServer
    {
    private:
        std::map<std::string,std::string> _config;
        // ToDO remove include open62541 
        UA_Server* _server;
        std::string _listening_url;
        const volatile UA_Boolean _is_running = true;
        
    public:
        DiscoveryServer(/* args */);
        ~DiscoveryServer();
        // ToDo use different key than string like "semaphore" 
        std::map<std::string,ServerConnector> KnownServers;
        void PrepareListeningOn(int port);
        void StartRunning();
        
    };
}