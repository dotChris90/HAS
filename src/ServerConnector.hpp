#pragma once 

#include <open62541/server_config_default.h>
#include <tuple>
#include <string>
#include <vector>
#include <tuple>

namespace HAS {
    class ServerConnector
    {
    private:
        // ToDo : remove lib specific 
        UA_Client* _client;
        std::string _url;
        bool is_on;
        // ToDo : must be multiple topics
        std::string _topic;
        bool _topics_known;
        int _size;
        std::string _sempaphorenfile;
    public:
        ServerConnector(/* args */);
        ~ServerConnector();
        void SetConnection(const std::string& connection_url);
        void SetSemaphore(const std::string& semaphore);
        void Connect();
        std::vector<std::tuple<std::string,int>> GetTopics();
        //std::tuple<std::string, int> GetAccessInterface(const std::string& interface);
        bool IsOn();
        bool HasTopics();
    };
    
}