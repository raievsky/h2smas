//
// Created by raievskc on 11/15/16.
//

#include <iostream>

#include "IncomingConnectionsManager.h"
#include "DummyEnvironment.h"

// TODO Use a serious logging framework
// TODO Integrate with a Qt GUI to show state
int main(int argc, char **argv)
{

    DummyEnvironment environment;
    AgentRequestHandler requestHandler(environment);

    try
    {
        boost::asio::io_service io_service;
        IncomingConnectionsManager server(io_service, requestHandler);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
