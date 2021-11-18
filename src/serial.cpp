
#include "serial.hpp"
#include "interface.h"

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
        return bla;
    }

}