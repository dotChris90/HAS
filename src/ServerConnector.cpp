#include "ServerConnector.hpp"
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include "utils.hpp"

namespace HAS
{
    ServerConnector::ServerConnector()
    {
        this->_topics_known = false;
        this->is_on = false;
    }

    ServerConnector::~ServerConnector() 
    {

    }

    void ServerConnector::SetConnection(const std::string& connection_url) 
    {
        //this->_client = UA_Client_new();
        //UA_(UA_Client_getConfig(this->_client));
        this->_url = connection_url;
    }
    
    void ServerConnector::SetSemaphore(const std::string& semaphore)
    {
        this->_sempaphorenfile = semaphore;
    }
    
    void ServerConnector::Connect()
    {
        this->_client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(this->_client));
        UA_StatusCode retval = UA_Client_connect(this->_client, this->_url.c_str());
    
        const UA_NodeId node_obj = UA_NODEID_NUMERIC(1,62540);
        const UA_NodeId node_meth = UA_NODEID_NUMERIC(1,62543);
        UA_QualifiedName ua_name;
        retval = UA_Client_readBrowseNameAttribute(this->_client,node_meth,&ua_name);
        retval = UA_Client_readBrowseNameAttribute(this->_client,node_obj,&ua_name);
        UA_Variant input[2];
        // TODo : remove CAN
        UA_String if_name = UA_STRING("CAN");
        UA_Int32 sample_rate = UA_Int32(1000);

        UA_Variant_init(input);
        UA_Variant_setScalarCopy(&input[0], &sample_rate, &UA_TYPES[UA_TYPES_INT32]);
        UA_Variant_setScalarCopy(&input[1], &if_name, &UA_TYPES[UA_TYPES_STRING]);
    
        size_t outputSize;
        UA_Variant *output;
        retval = UA_Client_call(this->_client, node_obj, node_meth, 2, input, &outputSize, &output);
    
        UA_String topic =  *(UA_String *) output[0].data;
        UA_Int64 size_struct = *(UA_Int64 *) output[1].data;

        this->_topic = FromUA2String(topic);
        this->_size = (int)size_struct;
        this->_topics_known = true;
    }

    std::vector<std::tuple<std::string,int>> ServerConnector::GetTopics()
    {
        std::vector<std::tuple<std::string,int>> topics;
        topics.push_back(std::make_tuple(this->_topic,this->_size));
        return topics;
    }

    bool ServerConnector::HasTopics()
    {
        return this->_topics_known;
    }
    
    bool ServerConnector::IsOn() 
    {
        return this->is_on;
    }
}