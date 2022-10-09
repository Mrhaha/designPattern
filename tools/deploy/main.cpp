
#include <cstdint>
#include <iostream>
#include "server.h"

int32_t main()
{
    try
    {
        server s("127.0.0.1", "6636");
        s.run();
    }catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
        return 0;
}