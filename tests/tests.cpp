//
// Created by raievskc on 12/14/16.
//

#include <AgentRequestHandler.h>
#include <DummyEnvironment.h>

// TODO Use a real testing framework...
int main(int argc, char **argv)
{

    DummyEnvironment env;
    AgentRequestHandler agentRequestHandler(env);

    agentRequestHandler.doSomeTests();

}

