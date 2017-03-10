//
// Created by raievskc on 12/5/16.
//

#include <iostream>
#include "DummyEnvironment.h"

// TODO unit test this function
std::vector<int> DummyEnvironment::getIdsInRange(int agentId, int range)
{
    std::vector<int> result;

    if (range > 0)
    {
        // TODO check that given agent id is valid.
        const Eigen::Vector2i pos = m_agentsPositions[agentId];
        const int squaredRange = range * range;

        std::cout << "Requesting agent position : [" << pos(0) << "," << pos(1) << "]" << std::endl;

        for (int i = 0; i < m_agentsPositions.size(); ++i)
        {
            std::cout << "Current agent position : [" << m_agentsPositions[i](0) << "," << m_agentsPositions[i](1)
                      << "]" << std::endl;

            if (i != agentId && (m_agentsPositions[i] - pos).squaredNorm() <= squaredRange)
            {
                std::cout << "Adding agent " << i << std::endl;
                result.push_back(i);
            }
        }
    }

    return std::move(result);
}

bool DummyEnvironment::setPos(int agentID, EnvironmentIF::coord_t x, EnvironmentIF::coord_t y, EnvironmentIF::coord_t z)
{
    // TODO check that given agent id is valid. And return false if it is not.

    m_agentsPositions[agentID](0) = x;
    m_agentsPositions[agentID](1) = y;

    return true;
}

bool DummyEnvironment::addAgent(int id)
{
    if (m_agentsPositions.size() <= id)
    {
        m_agentsPositions.resize(id + 1);
        m_agentsPositions[id](0) = 0;
        m_agentsPositions[id](1) = 0;

    }
    return false;
}

DummyEnvironment::~DummyEnvironment()
{

}
