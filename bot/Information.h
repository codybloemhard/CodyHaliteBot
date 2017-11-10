//
// Created by Cody on 10/11/2017.
//

#ifndef MYBOT_INFORMATION_H
#define MYBOT_INFORMATION_H

#include "hlt/entities.hpp"
#include "hlt/map.hpp"
#include "hlt/types.hpp"

namespace bot {
    class Information {
    private:
        hlt::PlayerId id;
        int round;
    public:
        std::vector<hlt::Planet> emptyPlanets, ourPlanets, enemyPlanets, notfullPlanets;
        Information(hlt::PlayerId id){
            this->id = id;
            round = 0;
        }
        void SortPlanets(const hlt::Map&);
        void EndCycle();
        hlt::Planet ClosestPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Planet OptimalPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        int GetRound();
        hlt::PlayerId GetPlayerID();
    };
}

#endif //MYBOT_INFORMATION_H