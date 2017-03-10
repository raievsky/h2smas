//
// Created by raievskc on 12/11/16.
//

#ifndef HHSMAS_ENVIRONMENTIF_H
#define HHSMAS_ENVIRONMENTIF_H

#include <vector>

class EnvironmentIF {

public:
    using coord_t = int;

    virtual bool addAgent(int id) = 0;

    virtual bool setPos(int agentID, coord_t x, coord_t y, coord_t z) = 0;

    virtual std::vector<int> getIdsInRange(int agentId, int range) = 0;

};

#endif //HHSMAS_ENVIRONMENTIF_H
