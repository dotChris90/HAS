
#include "serial.hpp"
#include "interface.h"
#include "CANInterface.hpp"

namespace bla
{
    serial::serial(/* args */) 
    {
        
    }
    
    serial::~serial() 
    {
        
    }
    
    std::string serial::serialData(std::string type, void* data) 
    {
        std::string bla = "";
        if (type.compare("/TEST") == 0) 
        {
            location* loc = (location*)data;
            bla += "x->";
            bla += std::to_string(loc->x);
            bla += ";";
            bla += "y->";
            bla += std::to_string(loc->y);
            bla += ";";
            bla += "z->";
            bla += std::to_string(loc->z);
            bla += ";";
        }
        if (type.compare("/SHM-CAN") == 0) 
        {
            interfaces::CANInterface* can_if = (interfaces::CANInterface*)data;
            bla += "{\"Type\":\"CAN_READ\",";
            bla += "\"id\":";
            bla += "\"" + std::to_string(can_if->Id) + "\",";
            bla += "\"data\":";
            bla += "\"" + std::to_string(can_if->Data) + "\"";
            bla += "}";
        }
        return bla;
    }

}