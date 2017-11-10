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
            RecordShipSpeed(map);

            hlt::Planet globalTarget;
            if(plan.GetStage() == STAGE::CONQUER){
                globalTarget = info.BiggestPlanet(info.enemyPlanets);
            }
            for(const auto ship : map.ships.at(info.GetPlayerID())){
                if(ship.docking_status == hlt::ShipDockingStatus::Docking) continue;
                hlt::Planet target;
                if(plan.GetStage() == STAGE::START){
                    target = info.OptimalPlanet(info.emptyPlanets, ship.pos);
                }
                else if(plan.GetStage() == STAGE::SETTLE){
                    target = info.OptimalPlanet(info.notfullPlanets, ship.pos);
                }
                else if(plan.GetStage() == STAGE::CONQUER){
                    target = info.OptimalPlanet(info.emptyPlanets, ship.pos);
                    if(target.pos == hlt::Vector(0,0)) target = info.OptimalPlanet(info.notfullPlanets, ship.pos);
                    if(target.pos == hlt::Vector(0,0)) target = globalTarget;
                }

                if(ship.can_dock(target)){
                    moves.push_back(hlt::Move::dock(ship.entity_id, target.entity_id));
                    continue;
                }
                const hlt::nullable<hlt::Move> move = hlt::navigation::navigate_ship_to_dock(map, ship, target, 7);
                if(move.second) moves.push_back(move.first);
            }
            info.EndCycle();
            plan.Step(map, info);
        }
    };
}

#endif //MYBOT_BOTALGO_H