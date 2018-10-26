//
// Created by raievskc on 12/5/16.
//

#ifndef HHSMAS_DUMMYENVIRONMENT_H
#define HHSMAS_DUMMYENVIRONMENT_H

#include <vector>
//#include "Position.h"
#include <Eigen/Dense>
#include "EnvironmentIF.h"

class DummyEnvironment : public EnvironmentIF {

public:
    virtual bool addAgent(int id);

    std::vector<int> getIdsInRange(int agentId, int range);

    virtual bool setPos(int agentID, coord_t x, coord_t y, coord_t z);

    virtual ~DummyEnvironment();

protected:
    //typedef Eigen::Vector2i pos_t __attribute__ ((mode (TI)));
    typedef Eigen::Vector2i pos_t;
    //typedef std::vector<pos_t, Eigen::aligned_allocator<pos_t> > posList_t __attribute__ ((mode (TI)));
    //typedef std::vector<pos_t> posList_t __attribute__ ((mode (TI)));
    typedef std::vector<pos_t> posList_t;
    posList_t m_agentsPositions;
    // std::vector<Eigen::Vector2i, Eigen::aligned_allocator<Eigen::Vector2i> > m_agentsPositions;

};

#endif //HHSMAS_DUMMYENVIRONMENT_H
