//
// Created by raievskc on 12/11/16.
//

#include <cstring>
#include <sstream>
#include <iostream>
#include "AgentRequestHandler.h"

const char *AgentRequestHandler::SET_POS_REQUEST_STRING = "setPos";
const char *AgentRequestHandler::AGTS_IN_RANGE_REQUEST_STRING = "agentsInRange";
const char *AgentRequestHandler::ADD_AGT_ID_RANGE_REQUEST_STRING = "addAgent";

AgentRequestHandler::AgentRequestHandler(EnvironmentIF& env)
        : m_environment(env)
{

}

bool AgentRequestHandler::handleRequest(int id, std::string request, std::string& answer)
{
    if (request.size() == 0)
    {
        return false;
    }

    if (request.compare(0, strlen(SET_POS_REQUEST_STRING), SET_POS_REQUEST_STRING) == 0)
    {
        return handleSetPos(id, request);
    }

    if (request.compare(0, strlen(AGTS_IN_RANGE_REQUEST_STRING), AGTS_IN_RANGE_REQUEST_STRING) == 0)
    {
        return handleAgentsInRange(id, request, answer);
    }

    if (request.compare(0, strlen(ADD_AGT_ID_RANGE_REQUEST_STRING), ADD_AGT_ID_RANGE_REQUEST_STRING) == 0)
    {
        return handleAddAgent(id);
    }
    return false;
}

// TODO Unit test this function
// TODO check its performance
bool AgentRequestHandler::handleSetPos(int agentId, const std::string& request)
{
    std::istringstream rsstream(request);

    rsstream.seekg(strlen(SET_POS_REQUEST_STRING) + 1); // +1 for the opening '('

    int x = 0, y = 0, z = 0;

    if (!rsstream.eof())
    {
        rsstream >> x;
    }

    if (rsstream.good() && !rsstream.eof())
    {
        rsstream.ignore(1); // Ignore ','
    }

    if (!rsstream.eof())
    {
        rsstream >> y;
    }

    if (rsstream.good() && !rsstream.eof())
    {
        rsstream.ignore(1); // Ignore ',', or ')' if request contains only x and y.
    }

    if (!rsstream.eof())  // Cover the case when the agent sent "setPos(x,y)" and not "setPos(x,y,z)"
    {
        rsstream >> z;
    }

    m_environment.setPos(agentId, x, y, z);

    return false;
}

bool AgentRequestHandler::handleAgentsInRange(int id, const std::string& request, std::string& answer)
{

    std::istringstream rsstream(request.substr(strlen(AGTS_IN_RANGE_REQUEST_STRING) + 1)); // +1 for the opening '('
    char discard;

    int range = -1;

    if (!rsstream.eof())
    {
        rsstream >> range;
    }

    std::vector<int> agtsInRange = m_environment.getIdsInRange(id, range);
    std::stringstream l_stringstream;
    l_stringstream << "[";
    for (auto id : agtsInRange)
    {
        l_stringstream << id << ",";
    }

    answer = l_stringstream.str();

    if (agtsInRange.size() > 0)
    {
        // Replace last ',' by ']'
        answer.back() = ']';
    }
    answer.push_back('\n');

    return true;
}

bool AgentRequestHandler::handleAddAgent(int id)
{
    m_environment.addAgent(id);
    return false;
}

void AgentRequestHandler::extractXYZ(const std::string request, int& x, int& y, int& z)
{
    std::istringstream iss(request);

    iss.ignore(request.size(), '(');
    iss >> x;
    iss.ignore(request.size(), ',');
    iss >> y;
    iss.ignore(request.size(), ',');
    if (!iss.eof())
    {
        iss >> z;
    }
}

bool AgentRequestHandler::testExtract(const std::string request, int x, int y, int z)
{
    int a = 0, b = 0, c = 0;

    extractXYZ(request, a, b, c);

    bool pass = true;

    if (a == x && b == y && c == z)
    {
        std::cout << "Extraction went fine." << std::endl;
    }
    else
    {
        fprintf(stderr, "Failure on extraction.\n");
        fprintf(stderr, "Expected : x: %d, y: %d, z: %d\n", x, y, z);
        fprintf(stderr, "Extracted : x: %d, y: %d, z: %d\n", a, b, c);
        pass = false;
    }

    return pass;
}

bool AgentRequestHandler::doSomeTests()
{
    bool success = true;
    success = success && testExtract("   setPos ( 12, 42);  ", 12, 42, 0);
    success = success && testExtract("setPos ( 32, 33, 43 );  ", 32, 33, 43);
    success = success && testExtract("  setPos  ( 32, -33, 43 );  ", 32, -33, 43);
    success = success && testExtract("   setPos ( 32, 33 );  ", 32, 33, 0);
    success = success && testExtract("setPos( 32, 33 )", 32, 33, 0);
    success = success && testExtract("setPos( 32, 33, )", 32, 33, 0);
    return success;
}
