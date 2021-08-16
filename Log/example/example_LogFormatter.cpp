#include "../Log.h"
#include <iostream>

using namespace tensir;

int main()
{
    LogFormatter formatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
}