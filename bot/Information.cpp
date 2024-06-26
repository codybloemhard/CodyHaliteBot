//
// Created by Cody on 10/11/2017.
//

#include "Information.h"

namespace bot{
    Information::Information(hlt::PlayerId id) {
        this->id = id;
        round = 0;
    }
    void Information::SortPlanets(const hlt::Map& map){
        allPlanets.clear();
        emptyPlanets.clear();
        ourPlanets.clear();
        enemyPlanets.clear();
        notfullPlanets.clear();
        for (const hlt::Planet &planet : map.planets) {
            allPlanets.push_back(planet);
            if(!planet.owned)
                emptyPlanets.push_back(planet);
            else if(planet.owner_id == id) {
                ourPlanets.push_back(planet);
                if(!planet.is_full())
                    notfullPlanets.push_back(planet);
            }
            else
                enemyPlanets.push_back(planet);
        }
    }
    void Information::SortEnemyShips(const hlt::Map& map) {
        enemys.clear();
        dockedEnemys.clear();
        for(const auto& player : map.ships) {
            if (player.first == id) continue;
            for (const auto &ship : player.second) {
                enemys.push_back(ship);
            }
        }
        for(const auto& planet : map.planets){
            enemysOnPlanet[planet.entity_id].clear();
            for(const auto& player : map.ships) {
                if(player.first == id) continue;
                for (const auto &ship : player.second) {
                    if (ship.docking_status != hlt::ShipDockingStatus::Undocked && ship.docked_planet == planet.entity_id) {
                        enemysOnPlanet[planet.entity_id].push_back(ship);
                        dockedEnemys.push_back(ship);
                    }
                }
            }
        }
        attackingEnemys.clear();
        for(const auto& player : map.ships) {
            if (player.first == id) continue;
            for (const auto &ship : player.second) {
                for(const auto &planet : ourPlanets){
                    if(ship.pos.dist(planet.pos) < 10){
                        attackingEnemys.push_back(ship);
                        break;
                    }
                }
            }
        }
    }
    void Information::RecordShipSpeed(const hlt::Map& map) {
        for(const auto &pair : map.ships){
            for(const auto ship : pair.second){
                shipSpeeds[ship.entity_id] = prevShipPos[ship.entity_id] - ship.pos;
            }
        }
        for(const auto &pair : map.ships){
            for(const auto ship : pair.second){
                prevShipPos[ship.entity_id] = ship.pos;
            }
        }
    }
    void Information::EndCycle() {
        round++;
    }
    hlt::Ship Information::ClosestAttacker(hlt::Vector pos){
        hlt::Ship res = hlt::Ship();
        double min = 1000000;
        for(const auto& ship : attackingEnemys){
            double dist = ship.pos.dist(pos);
            if(min > dist){
                min = dist;
                res = ship;
            }
        }
        return res;
    }
    hlt::Ship Information::ShipOnPlanet(hlt::Planet& planet) {
        if(enemysOnPlanet[planet.entity_id].size() == 1) return enemysOnPlanet[planet.entity_id][0];
        if(enemysOnPlanet[planet.entity_id].size() > 1) return enemysOnPlanet[planet.entity_id][rand() % (enemysOnPlanet[planet.entity_id].size() - 1)];
        else return hlt::Ship();
    }
    hlt::Ship Information::ClosestEnemy(hlt::Vector pos) {
        if(enemys.size() == 0) return hlt::Ship();
        if(enemys.size() == 1){
            if(enemys[0].docking_status == hlt::ShipDockingStatus::Undocked) return hlt::Ship();
            else return enemys[0];
        }
        double min = 100000;
        hlt::Ship mins = hlt::Ship();
        for(const auto& ship : enemys){
            double dist = ship.pos.dist(pos);
            if(dist < min){
                min = dist;
                mins = ship;
            }
        }
        return mins;
    }
    hlt::Ship Information::ClosestEnemyOnPlanet(hlt::Vector pos) {
        if(dockedEnemys.size() == 0) return hlt::Ship();
        if(dockedEnemys.size() == 1){
            if(dockedEnemys[0].docking_status == hlt::ShipDockingStatus::Undocked) return hlt::Ship();
            else return dockedEnemys[0];
        }
        double min = 100000;
        hlt::Ship mins = hlt::Ship();
        for(const auto& ship : dockedEnemys){
            double dist = ship.pos.dist(pos);
            if(dist < min){
                min = dist;
                mins = ship;
            }
        }
        return mins;
    }
    hlt::Planet Information::ClosestPlanet(std::vector<hlt::Planet>& source, hlt::Vector pos){
        double min = 100000000;
        hlt::Planet minp;
        for(const auto &planet : source){
            hlt::Vector diff = planet.pos - pos;
            double len = diff.length();
            if(len < min){
                min = len;
                minp = planet;
            }
        }
        return minp;
    }
    hlt::Planet Information::OptimalPlanet(std::vector<hlt::Planet>& source, hlt::Vector pos){
        double max = 0;
        hlt::Planet maxp;
        for(const auto &planet : source){
            hlt::Vector diff = planet.pos - pos;
            double len = diff.length();
            double rad = planet.radius;
            double score = rad * rad / len;
            if(score > max){
                max = score;
                maxp = planet;
            }
        }
        return maxp;
    }
    void Information::OptimalPlanet(std::vector<hlt::Planet>& dest, std::vector<hlt::Planet>& source, hlt::Vector pos, unsigned int size) {
        std::vector<double> max;
        std::vector<hlt::Planet> maxp;
        for(int i = 0; i < size; i++){
            max.push_back(0);
            maxp.push_back(hlt::Planet());
        }
        for(const auto& planet : source){
            double len = pos.dist(planet.pos);
            double rad = planet.radius;
            double cost = rad * rad / len;
            for(int i = 0; i < size; i++){
                if(cost > max[i]){
                    max[i] = cost;
                    maxp[i] = planet;
                    break;
                }
            }
        }
        for(auto& planet : maxp)
            if(!IsNull(planet))
                dest.push_back(planet);
    }
    hlt::Planet Information::OptimalPlanet(hlt::Vector pos, hlt::Planet& planetA, hlt::Planet& planetB, double scalarA, double scalarB) {
        if(IsNull(planetA)) return planetB;
        if(IsNull(planetB)) return planetA;
        double scoreA = scalarA * 2 * planetA.radius / planetA.pos.dist(pos);
        double scoreB = scalarB * 2 * planetB.radius / planetB.pos.dist(pos);
        return scoreA > scoreB ? planetA : planetB;
    }
    hlt::Planet Information::BiggestPlanet(std::vector<hlt::Planet>& source) {
        double max = 0;
        hlt::Planet maxp;
        for(const auto &planet : source){
            double size = planet.radius;
            if(size > max){
                max = size;
                maxp = planet;
            }
        }
        return maxp;
    }
    double Information::PowerPart(hlt::Map& map) {
        double our = 0, enemy = 0;
        for(const auto& planet : map.planets){
            if(planet.owner_id == id) our += planet.radius;
            else enemy += planet.radius;
        }
        return our / (our + enemy);
    }
    hlt::Vector Information::CenterOfGravity(std::vector<hlt::Planet> dudes){
        hlt::Vector endpos;
        for(const auto& ding : dudes)
            endpos = endpos + ding.pos;
        endpos = endpos / dudes.size();
        return endpos;
    }
    hlt::Vector Information::CenterOfGravity(std::vector<hlt::Ship> dudes){
        hlt::Vector endpos;
        for(const auto& ding : dudes)
            endpos = endpos + ding.pos;
        endpos = endpos / dudes.size();
        return endpos;
    }
    hlt::Planet Information::PickRandom(std::vector<hlt::Planet>& planets){
        if(planets.empty()) return hlt::Planet();
        unsigned long int max = planets.size() - 1;
        if(max < 0) return hlt::Planet();
        if(max == 0) return planets[0];
        unsigned long int r = rand() % max;
        return planets[r];
    }
    unsigned int Information::PlayersInRoom(const hlt::Map& map){
        unsigned int count = 0;
        for(const auto& player : map.ships){
            count++;
        }
        return count;
    }
    int Information::GetRound() { return round; }
    hlt::PlayerId Information::GetPlayerID() { return id; }
    bool Information::IsNull(const hlt::Entity& e) const{ return e.pos == hlt::Vector(0,0); }
}