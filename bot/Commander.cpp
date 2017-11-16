//
// Created by Cody on 11/11/2017.
//

#include "Commander.h"
#include "../hlt/log.hpp"

namespace bot{
    void Commander::Command(hlt::Map& map, std::vector<hlt::Move>& moves, Information& info, GamePlan& plan){
        for(auto& ship : map.ships.at(info.GetPlayerID())){
            if(available.find(ship.entity_id) == available.end()) {
                Task task = Task(-1, TASKTYPE::NOOP);
                task.ship = ship;
                available[ship.entity_id] = task;
            }
        }
        GenerateTasks(map, info, plan);
        DelegateTasks(info, plan);
        UpdateShips(map, info.GetPlayerID());
        ControlShips(map, moves, info);
    }
    void Commander::ControlShips(hlt::Map& map, std::vector<hlt::Move>& moves, Information& info){
        for(auto& ship : available){
            if(ship.second.active != 9) continue;
            hlt::Ship _ship = ship.second.ship;
            switch (ship.second.type){
                case TASKTYPE::CAPTURE:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    if(info.IsNull(ship.second.target)){
                        FreeShip(ship.first);
                        continue;
                    }
                    if(FlyToPlanetRandom(map, _ship, ship.second.target, moves));
                    else FreeShip(ship.first);
                    break;
                }
                case TASKTYPE::AID:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    if(info.IsNull(ship.second.target)){
                        FreeShip(ship.first);
                        continue;
                    }
                    if(FlyToPlanet(map, _ship, ship.second.target, moves));
                    else FreeShip(ship.first);
                    break;
                }
                case TASKTYPE::KILL:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    if(info.IsNull(ship.second.target)){
                        FreeShip(ship.first);
                        continue;
                    }
                    hlt::Ship victim = info.ShipOnPlanet(ship.second.target);
                    if(info.IsNull(victim)){
                        FreeShip(ship.first);
                        continue;
                    }else{
                        if(FlyToShip(map, _ship, victim, moves));
                        else FreeShip(ship.first);
                    }
                    break;
                }
                case TASKTYPE::SNIPE:{
                    hlt::Ship victim = info.ClosestEnemy(_ship.pos);
                    if(info.IsNull(victim)) FreeShip(ship.first);
                    else FlyToShip(map, _ship, victim, moves);
                    break;
                }
                case TASKTYPE::CRASH:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    if(info.IsNull(ship.second.target)){
                        FreeShip(ship.first);
                        continue;
                    }
                    if(info.IsNull(ship.second.target)) FreeShip(ship.first);
                    else FlyToSuicide(map, _ship, ship.second.target, moves);
                    break;
                }
                default:{
                    FreeShip(ship.first);
                }
            }
        }
    }
    void Commander::GenerateTasks(hlt::Map& map, Information& info, GamePlan& plan){
        unsigned int coins = 0;
        for(const auto& ship : available){
            if(ship.second.active == -1) coins++;
        }
        if(coins <= 0) return;
        switch(plan.GetStage()){
            case STAGE::START:{
                hlt::Vector pos(0,0);
                for(const auto& ship : available){
                    if(ship.second.active == -1){
                        pos = ship.second.ship.pos;
                        break;
                    }
                }
                std::vector<hlt::Planet> best;
                info.OptimalPlanet(best, info.emptyPlanets, pos, 2);
                for(int i = 0; i < coins; i++){
                    hlt::Planet pick = info.PickRandom(best);
                    Task task = Task(1, TASKTYPE::CAPTURE);
                    task.target = pick;
                    tasks.push_back(task);
                }
                break;
            }
            case STAGE::SETTLE:{
                hlt::Vector pos = info.CenterOfGravity(info.ourPlanets);
                std::vector<hlt::Planet> best;
                info.OptimalPlanet(best, info.emptyPlanets, pos, 2);
                info.OptimalPlanet(best, info.notfullPlanets, pos, 2);
                for(int i = 0; i < coins; i++){
                    hlt::Planet pick = info.PickRandom(best);
                    Task task = Task(1, TASKTYPE::AID);
                    task.target = pick;
                    tasks.push_back(task);
                }
                break;
            }
            case STAGE::CONQUER:{

                break;
            }
            case STAGE::DESTROY:{

                break;
            }
            default:{

                break;
            }
        }
    }
    void Commander::DelegateTasks(Information& info, GamePlan& plan){
        unsigned int coins = 0;
        for(const auto& ship : available){
            if(ship.second.active == -1) coins++;
        }
        if(tasks.empty()) return;
        for(auto& task : tasks){
            if(coins <= 0) break;
            double mindist = 1000000000.0;
            hlt::Ship min = hlt::Ship();
            for(auto& ship : available){
                if(ship.second.active == 9) continue;
                hlt::Ship _ship = ship.second.ship;
                double dist = _ship.pos.dist(task.target.pos);
                if(dist < mindist){
                    mindist = dist;
                    min = ship.second.ship;
                }
            }
            if(!info.IsNull(min)) {
                coins--;
                CommandShip(min, task);
            }
        }
        tasks.clear();
    }
    void Commander::UpdateShips(hlt::Map& map, hlt::PlayerId id){
        for(const auto& ship : map.ships.at(id)){
            if(available[ship.entity_id].active == 9)
                available[ship.entity_id].ship = ship;
        }
    }
    void Commander::CommandShip(hlt::Ship& ship, Task task) {
        if(available[ship.entity_id].active > 0) return;
        task.active = 9;
        task.ship = ship;
        available[ship.entity_id] = task;
    }
    bool Commander::ControlDocking(const hlt::Ship& ship, Information& info, std::vector<hlt::Move>& moves){
        if(IsDocking(ship)) return true;
        hlt::Planet target = info.ClosestPlanet(info.allPlanets, ship.pos);
        return FlyToDock(ship, target, moves);
    }
    void Commander::FreeShip(unsigned int id) {
        available[id].active = -1;
        available[id].target = hlt::Planet();
        tasks.push_back(available[id]);
    }
}