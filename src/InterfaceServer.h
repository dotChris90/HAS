#include <map>
#include <string>

namespace HAS
{
    class InterfaceServer
    {
    private:
        /* data */
    public:
        ~InterfaceServer() {};
        void ConfigureServer(std::map<std::string,std::string> config);

    };
        
}