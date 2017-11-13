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
        std::unordered_map<unsigned int, hlt::Vector> prevShipPos, shipSpeeds;
    public:
        std::vector<hlt::Planet> allPlanets, emptyPlanets, ourPlanets, enemyPlanets, notfullPlanets;
    public:
        Information(hlt::PlayerId id);
        void SortPlanets(const hlt::Map&);
        void SortEnemyShips(const hlt::Map&);
        void RecordShipSpeed(const hlt::Map&);
        void EndCycle();
        hlt::Ship ShipOnPlanet(hlt::Planet&);
        hlt::Planet ClosestPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Planet OptimalPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Planet OptimalPlanet(hlt::Vector, hlt::Planet&, hlt::Planet&, double, double);
        hlt::Planet BiggestPlanet(std::vector<hlt::Planet>&);
        int GetRound();
        hlt::PlayerId GetPlayerID();
        bool IsNull(const hlt::Entity& e) const{
            return e.pos == hlt::Vector(0,0);
        }
    };
}

#endif //MYBOT_INFORMATION_H