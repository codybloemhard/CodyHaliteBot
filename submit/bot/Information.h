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
        std::unordered_map<unsigned int, std::vector<hlt::Ship>> enemysOnPlanet;
        std::unordered_map<unsigned int, hlt::Vector> prevShipPos, shipSpeeds;
        std::vector<hlt::Ship> enemys, dockedEnemys, attackingEnemys;
    public:
        std::vector<hlt::Planet> allPlanets, emptyPlanets, ourPlanets, enemyPlanets, notfullPlanets;
    public:
        Information(hlt::PlayerId id);
        void SortPlanets(const hlt::Map&);
        void SortEnemyShips(const hlt::Map&);
        void RecordShipSpeed(const hlt::Map&);
        void EndCycle();
        hlt::Ship ClosestAttacker(hlt::Vector);
        hlt::Ship ShipOnPlanet(hlt::Planet&);
        hlt::Ship ClosestEnemy(hlt::Vector);
        hlt::Planet ClosestPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        hlt::Ship ClosestEnemyOnPlanet(hlt::Vector);
        hlt::Planet OptimalPlanet(std::vector<hlt::Planet>&, hlt::Vector);
        void OptimalPlanet(std::vector<hlt::Planet>&, std::vector<hlt::Planet>&, hlt::Vector, unsigned int);
        hlt::Planet OptimalPlanet(hlt::Vector, hlt::Planet&, hlt::Planet&, double, double);
        hlt::Planet BiggestPlanet(std::vector<hlt::Planet>&);
        double PowerPart(hlt::Map&);
        hlt::Vector CenterOfGravity(std::vector<hlt::Planet> dudes);
        hlt::Vector CenterOfGravity(std::vector<hlt::Ship> dudes);
        hlt::Planet PickRandom(std::vector<hlt::Planet>&);
        int GetRound();
        unsigned int PlayersInRoom(const hlt::Map&);
        hlt::PlayerId GetPlayerID();
        bool IsNull(const hlt::Entity&) const;
    };
}

#endif //MYBOT_INFORMATION_H