//
// Created by Cody on 10/11/2017.
//

#ifndef MYBOT_INFORMATION_H
#define MYBOT_INFORMATION_H

#include "../hlt/entities.hpp"
#include "../hlt/map.hpp"
#include "../hlt/types.hpp"

namespace bot {
    class Information {
    private:
        hlt::PlayerId id;
        int round;
        std::unordered_map<unsigned int, std::vector<hlt::Ship>> enemys;
    public:
        std::vector<hlt::Planet> emptyPlanets, ourPlanets, enemyPlanets, notfullPlanets;
        Information(hlt::PlayerId id);
        void SortPlanets(const hlt::Map&);
        void SortEnemyShips(const hlt::Map&);
        void EndCycle();
        hlt::Ship ShipOnPlanet(hlt::Planet&);
        hlt::Planet ClosestPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Planet OptimalPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Planet BiggestPlanet(std::vector<hlt::Planet>&);
        int GetRound();
        hlt::PlayerId GetPlayerID();
    };
}

#endif //MYBOT_INFORMATION_H