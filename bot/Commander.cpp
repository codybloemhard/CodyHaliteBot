//
// Created by Cody on 11/11/2017.
//

#include "Commander.h"
#include "../hlt/log.hpp"

namespace bot{
    void Commander::Command(hlt::Map& map, std::vector<hlt::Move>& moves, Information& info, GamePlan& plan){
        switch (plan.GetStage()){
            case STAGE::START:{
                for(const auto& ship : map.ships.at(info.GetPlayerID())){
                    CommandShip(ship, TASKTYPE::CAPTURE);
                }
                break;
            }
            case STAGE::SETTLE:{
                for(const auto& ship : map.ships.at(info.GetPlayerID())){
                    CommandShip(ship, TASKTYPE::AID);
                }
                break;
            }
            case STAGE::CONQUER:{
                for(const auto& ship : map.ships.at(info.GetPlayerID())){
                    CommandShip(ship, TASKTYPE::KILL);
                }
                break;
            }
        }
        ControlShips(map, moves, info);
    }
    void Commander::ControlShips(hlt::Map& map, std::vector<hlt::Move>& moves, Information& info){
        for(auto& ship : available){
            if(ship.second.active == 0) continue;
            hlt::Ship _ship = ship.second.ship;
            if(ControlDocking(_ship, info, moves)) continue;
            switch (ship.second.type){
                case TASKTYPE::CAPTURE:{
                    hlt::Planet capTarget = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet aidTarget = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet target = info.OptimalPlanet(_ship.pos, capTarget, aidTarget, 2, 1);
                    if(info.IsNull(target)) ship.second.type = TASKTYPE::KILL;
                    else FlyToPlanet(map, _ship, target, moves);
                    break;
                }
                case TASKTYPE::AID:{
                    hlt::Planet capTarget = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet aidTarget = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet target = info.OptimalPlanet(_ship.pos, capTarget, aidTarget, 1, 2);
                    if(info.IsNull(target)) ship.second.type = TASKTYPE::KILL;
                    else FlyToPlanet(map, _ship, target, moves);
                    break;
                }
                case TASKTYPE::KILL:{
                    hlt::Planet capTarget = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet aidTarget = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet killTarget = info.ClosestPlanet(info.enemyPlanets, _ship.pos);
                    hlt::Ship victim = info.ShipOnPlanet(killTarget);
                    hlt::Planet target = info.OptimalPlanet(_ship.pos, capTarget, aidTarget, 1, 1);
                    if(info.IsNull(victim)) FlyToPlanet(map, _ship, target, moves);
                    else{
                        target = info.OptimalPlanet(_ship.pos, killTarget, target, 1, 1);
                        if(target.owner_id == info.GetPlayerID() || !target.owned) FlyToPlanet(map, _ship, target, moves);
                        else FlyToShip(map, _ship, victim, moves);
                    }
                    break;
                }
                default:{
                    FreeShip(ship.first);
                }
            }
        }
    }
    void Commander::CommandShip(const hlt::Ship& ship, TASKTYPE type) {
        if(available[ship.entity_id].active > 0){
            available[ship.entity_id].ship = ship;
            return;
        }
        available[ship.entity_id] = Task(10, type, ship);
    }
    bool Commander::ControlDocking(const hlt::Ship& ship, Information& info, std::vector<hlt::Move>& moves){
        if(IsDocking(ship)) return true;
        hlt::Planet target = info.ClosestPlanet(info.allPlanets, ship.pos);
        return FlyToDock(ship, target, moves);
    }
    void Commander::FreeShip(const unsigned int id) {
        available[id].active = 0;
    }
}