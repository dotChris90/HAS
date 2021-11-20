#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <sys/mman.h>
#include <fcntl.h>
#include "interface.h"

#include <signal.h>

#include <string>

static UA_StatusCode
activeCycleIfMethodCallback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {
    
    UA_Int32* cycleTime = (UA_Int32*)input[0].data;
    UA_String* if_name = (UA_String*)input[1].data;

    std::string shm_topic = std::string("/SHM-") + std::string((char *)if_name->data);  
    UA_String shm_topic_ua = UA_STRING_ALLOC(shm_topic.c_str());

    UA_Int64 size_shm = 9876;

    UA_Variant_setScalarCopy(&output[0], &shm_topic_ua, &UA_TYPES[UA_TYPES_STRING]);
    UA_Variant_setScalarCopy(&output[1], &size_shm, &UA_TYPES[UA_TYPES_INT64]);
    
    UA_String_clear(&shm_topic_ua);
    UA_Int64_clear(&size_shm);
    
    return UA_STATUSCODE_GOOD;
}


static void
addActiveCycleIfMMethod(UA_Server *server) 
{
    UA_Argument inputArguments[2];
    UA_Argument_init(&inputArguments[0]);
    inputArguments[0].description = UA_LOCALIZEDTEXT("en-US", "Cyclic time");
    inputArguments[0].name = UA_STRING("int32");
    inputArguments[0].dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    inputArguments[0].valueRank = UA_VALUERANK_SCALAR;

    UA_Argument_init(&inputArguments[1]);
    inputArguments[1].description = UA_LOCALIZEDTEXT("en-US", "Interface name");
    inputArguments[1].name = UA_STRING("string");
    inputArguments[1].dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    inputArguments[1].valueRank = UA_VALUERANK_SCALAR;

    UA_Argument outputArgument[2];
    UA_Argument_init(&outputArgument[0]);
    outputArgument[0].description = UA_LOCALIZEDTEXT("en-US", "shared Memory location");
    outputArgument[0].name = UA_STRING("a string");
    outputArgument[0].dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument[0].valueRank = UA_VALUERANK_SCALAR;

    UA_Argument_init(&outputArgument[1]);
    outputArgument[1].description = UA_LOCALIZEDTEXT("en-US", "size of shm");
    outputArgument[1].name = UA_STRING("int64");
    outputArgument[1].dataType = UA_TYPES[UA_TYPES_INT64].typeId;
    outputArgument[1].valueRank = UA_VALUERANK_SCALAR;

    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.description = UA_LOCALIZEDTEXT("en-US","Activate cyclic update of IF");
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","Activate cyclic update of IF");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1,62541),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "Activate cyclic IF"),
                            helloAttr, &activeCycleIfMethodCallback,
                            2, inputArguments, 2, outputArgument, NULL, NULL);

}

void addmanualCAN(UA_Server *server) {
    
    UA_NodeId CANReadId; /* get the nodeid assigned by the server */
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CAN_0_READ");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, "CAN_0_READ"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            oAttr, NULL, &CANReadId);

    UA_VariableAttributes msgIdAttr = UA_VariableAttributes_default;
    UA_Byte msgId = ' ';
    UA_Variant_setScalar(&msgIdAttr.value, &msgId, &UA_TYPES[UA_TYPES_BYTE]);
    msgIdAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CAN_0_ID");
    UA_Server_addVariableNode(server, UA_NODEID_NULL, CANReadId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "CAN_0_ID"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), msgIdAttr, NULL, NULL);

    UA_VariableAttributes msgDataAttr = UA_VariableAttributes_default;
    UA_Int64 msgData = 12345678;
    UA_Variant_setScalar(&msgDataAttr.value, &msgData, &UA_TYPES[UA_TYPES_INT64]);
    msgDataAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CAN_0_DATA");
    UA_Server_addVariableNode(server, UA_NODEID_NULL, CANReadId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "CAN_0_DATA"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), msgDataAttr, NULL, NULL);
    /*
    int fd = shm_open("/TEST", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        perror("open");
    int size_struct = sizeof(location);
    int res = ftruncate(fd,size_struct);
    void* addr = mmap(NULL,size_struct,PROT_WRITE,MAP_SHARED,fd,0);
    location* my_struct = (location*) addr;
    my_struct->x = 5.0;
    my_struct->y = -5.0;
    my_struct->z = 3456.8175;
    */
}

static volatile UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(void) {
    
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    addmanualCAN(server);
    addActiveCycleIfMMethod(server);
    
    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}