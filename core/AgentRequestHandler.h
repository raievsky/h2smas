//
// Created by raievskc on 12/11/16.
//

#include <string>
#include "EnvironmentIF.h"

#ifndef HHSMAS_AGENTREQUESTHANDLER_H
#define HHSMAS_AGENTREQUESTHANDLER_H

class AgentRequestHandler {

public:
    static const char *SET_POS_REQUEST_STRING;
    static const char *AGTS_IN_RANGE_REQUEST_STRING;
    static const char *ADD_AGT_ID_RANGE_REQUEST_STRING;

    AgentRequestHandler(EnvironmentIF &env);

    bool handleRequest(int id, std::string request, std::string &answer);

    bool doSomeTests();

protected:
    EnvironmentIF &m_environment;

    bool handleSetPos(int agentId, const std::string &basic_string);

    bool handleAgentsInRange(int id, const std::string &request, std::string &answer);

    bool handleAddAgent(int id);

    void extractXYZ(const std::string request, int &x, int &y, int &z);

    void testExtract(const std::string request, int x, int y, int z);

};

#endif //HHSMAS_AGENTREQUESTHANDLER_H
