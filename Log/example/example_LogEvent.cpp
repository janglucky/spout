#include "../Log.h"
#include <iostream>

using namespace tensir;

int main()
{
    LogEvent log;

    int a = 10;
    char ch = 'A';
    std::string str = "hello world!";

    log.format("测试一下：%d %c %s %p\n", a, ch, str.c_str(), &a);

    std::cout<<log.getContent()<<std::endl;

    return 0;

}