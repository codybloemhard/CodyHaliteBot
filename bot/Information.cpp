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
        emptyPlanets.clear();
        ourPlanets.clear();
        enemyPlanets.clear();
        notfullPlanets.clear();
        for (const hlt::Planet &planet : map.planets) {
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
        for(const auto& player : map.ships){
            if(player.first == id) continue;
            for(const auto& planet : map.planets){
                enemys[planet.entity_id].clear();
                for(const auto& ship : player.second){
                    double dist = (planet.pos - ship.pos).length();
                    if(dist < 10) enemys[planet.entity_id].push_back(ship);
                }
            }
        }
    }
    void Information::EndCycle() {
        round++;
    }
    hlt::Ship Information::ShipOnPlanet(hlt::Planet& planet) {
        if(enemys.size() > 0) return enemys[planet.entity_id][0];
        else return hlt::Ship();
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
    hlt::Planet Information::OptimalPlanet(std::vector<hlt::Planet>& source, hlt::Vector pos) {
        double max = 0;
        hlt::Planet maxp;
        for(const auto &planet : source){
            hlt::Vector diff = planet.pos - pos;
            double len = diff.length();
            double size = planet.docking_spots;
            double cost = size * size / len;
            if(cost > max){
                max = cost;
                maxp = planet;
            }
        }
        return maxp;
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
    int Information::GetRound() { return round; }
    hlt::PlayerId Information::GetPlayerID() { return id; }
}