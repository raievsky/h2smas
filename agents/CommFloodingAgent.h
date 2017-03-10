//
// Created by raievskc on 11/13/16.
//

#ifndef HHSMAS_AGENT_H
#define HHSMAS_AGENT_H

#include <vector>

class CommFloodingAgent {

    int m_id;
    int m_socketfd;

    std::vector<int> m_agentList;

    static const int BUF_SIZE = 4096;
    char m_inputBuf[BUF_SIZE];

    bool m_initializationSucceeded = false;

protected:
    bool connectToCore(const char *node, const char *service);

    bool retrieveAgentsList();

public:
    CommFloodingAgent();

    ~CommFloodingAgent();

    bool init(const char *host, const char *port);

    void run();

    void sendMessage(int receiverId, const char *message);

    void retrieveAgentsInRange(int range);

    void printAllRead();
};

#endif //HHSMAS_AGENT_H
