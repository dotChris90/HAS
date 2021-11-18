#ifndef SERIAL
#define SERIAL

#include <string>

namespace bla {
    class serial
    {
    private:
        /* data */
    public:
        serial(/* args */);
        ~serial();
        static std::string serialData(std::string type, void* data);
    };

}

#endif