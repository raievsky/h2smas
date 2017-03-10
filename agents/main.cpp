#include <iostream>
#include "CommFloodingAgent.h"

int main()
{
    std::cout << "Hello, World!" << std::endl;

    CommFloodingAgent commFloodingAgent;
    // commFloodingAgent.init("127.0.0.1", "7777");
    commFloodingAgent.init("localhost", "7777");
    commFloodingAgent.run();

    return 0;
}