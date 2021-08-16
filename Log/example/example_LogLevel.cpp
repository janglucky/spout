#include "../Log.h"
#include <iostream>

using namespace tensir;

int main()
{
    LogLevel log;
    
    std::cout<<log.toString(LogLevel::DEBUG)<<std::endl;

    std::cout<<log.fromString("debug")<<std::endl;
    return 0;

}