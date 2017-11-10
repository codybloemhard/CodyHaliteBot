//
// Created by Cody on 09/11/2017.
//
#ifndef MYBOT_BOTALGO_H
#define MYBOT_BOTALGO_H

#include "../hlt/types.hpp"
#include "../hlt/navigation.hpp"
#include "Information.h"
#include "GamePlan.h"

namespace bot{
    class BotAlgo{
    private:
        Information info;
        GamePlan plan;
        std::unordered_map<unsigned int, hlt::Vector> prevShipPos, shipSpeeds;
        void RecordShipSpeed(const hlt::Map& map){
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
    public:
        BotAlgo(hlt::PlayerId id) : info(Information(id)), plan(GamePlan()){
        }
        void Moves(hlt::Map& map, std::vector<hlt::Move>& moves){
            info.SortPlanets(map);
            info.SortEnemyShips(map);
            RecordShipSpeed(map);

            for(const auto ship : map.ships.at(info.GetPlayerID())){
                if(ship.docking_status == hlt::ShipDockingStatus::Docking) continue;
                hlt::Planet targetP;
                hlt::Ship targetS;
                if(plan.GetStage() == STAGE::START){
                    targetP = info.OptimalPlanet(info.emptyPlanets, ship.pos);
                }
                else if(plan.GetStage() == STAGE::SETTLE){
                    targetP = info.OptimalPlanet(info.notfullPlanets, ship.pos);
                }
                else if(plan.GetStage() == STAGE::CONQUER){
                    targetP = info.OptimalPlanet(info.emptyPlanets, ship.pos);
                    if(targetP.pos == hlt::Vector(0,0)) targetP = info.OptimalPlanet(info.notfullPlanets, ship.pos);
                    if(targetP.pos == hlt::Vector(0,0)) {
                        targetP = info.OptimalPlanet(info.enemyPlanets, ship.pos);
                        for (const auto &player : map.ships) {
                            targetS = info.ShipOnPlanet(targetP);
                            break;
                        }
                    }
                }

                if(!(targetS.pos == hlt::Vector(0,0))){
                    hlt::nullable<hlt::Move> m = hlt::navigation::navigate_ship_towards_target(map, ship, targetS.pos, 7, true, hlt::constants::MAX_NAVIGATION_CORRECTIONS, M_PI/180);
                    if(m.second)moves.push_back(m.first);
                }
                else if(ship.can_dock(targetP)){
                    moves.push_back(hlt::Move::dock(ship.entity_id, targetP.entity_id));
                    continue;
                }
                else {
                    const hlt::nullable<hlt::Move> move = hlt::navigation::navigate_ship_to_dock(map, ship, targetP, 7);
                    if (move.second) moves.push_back(move.first);
                }
            }
            info.EndCycle();
            plan.Step(map, info);
        }
    };
}

#endif //MYBOT_BOTALGO_H