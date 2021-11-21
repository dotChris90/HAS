#ifndef CAN_IF
#define CAN_IF

#include <cstdint>

namespace interfaces {
    struct CANInterface
    {
        char Id;
        int64_t Data;
    };
    
}

#endif