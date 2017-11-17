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
        if(plan.DidChange()) round = 0;
        if(round >= 10)  round = 0;
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
                    hlt::Planet target = info.ClosestPlanet(info.notfullPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = info.ClosestPlanet(info.emptyPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    if(info.IsNull(ship.second.target) || ship.second.target.owned){
                        FreeShip(ship.first);
                        continue;
                    }
                    if(FlyToPlanetRandom(map, _ship, ship.second.target, moves));
                    else FreeShip(ship.first);
                    break;
                }
                case TASKTYPE::AID:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    hlt::Planet target = info.ClosestPlanet(info.notfullPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = info.ClosestPlanet(info.emptyPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    if(info.IsNull(ship.second.target) || ship.second.target.owner_id != info.GetPlayerID()){
                        FreeShip(ship.first);
                        continue;
                    }
                    if(FlyToPlanet(map, _ship, ship.second.target, moves));
                    else FreeShip(ship.first);
                    break;
                }
                case TASKTYPE::KILL:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    hlt::Planet target = info.ClosestPlanet(info.notfullPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = info.ClosestPlanet(info.emptyPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = ship.second.target;
                    if(info.IsNull(target)){
                        target = info.ClosestPlanet(info.enemyPlanets, _ship.pos);
                    }
                    hlt::Ship victim = info.ShipOnPlanet(target);
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
                    hlt::Ship victim = info.ClosestEnemyOnPlanet(_ship.pos);
                    if(info.IsNull(victim)) victim = info.ClosestEnemy(_ship.pos);
                    else FlyToShip(map, _ship, victim, moves);
                    break;
                }
                case TASKTYPE::CRASH:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    hlt::Planet target = info.ClosestPlanet(info.notfullPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = info.ClosestPlanet(info.emptyPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 25 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    if(!info.IsNull(ship.second.target)) FlyToSuicide(map, _ship, ship.second.target, moves);
                    FreeShip(ship.first);//always realocate
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
                pos = info.CenterOfGravity(map.ships.at(info.GetPlayerID()));
                std::vector<hlt::Planet> best;
                info.OptimalPlanet(best, info.emptyPlanets, pos, 2);
                bool _1v1 = info.PlayersInRoom(map) == 2;
                for(int i = 0; i < coins; i++){
                    if(round < 1 && _1v1){
                        Task task = Task(1, TASKTYPE::SNIPE);
                        tasks.push_back(task);
                        round++;
                    }else{
                        int j = i;
                        if(j >= 2) j = 0;
                        hlt::Planet pick = best[j];
                        Task task = Task(1, TASKTYPE::CAPTURE);
                        task.target = pick;
                        tasks.push_back(task);
                        round++;
                    }
                }
                break;
            }
            case STAGE::CONQUER:{
                hlt::Vector pos = info.BiggestPlanet(info.ourPlanets).pos;
                std::vector<hlt::Planet> bestCap, bestAid;
                info.OptimalPlanet(bestAid, info.notfullPlanets, pos, 2);
                info.OptimalPlanet(bestCap, info.emptyPlanets, pos, 2);
                for(int i = 0; i < coins; i++){
                    hlt::Planet pick;
                    TASKTYPE type;
                    if(round < 2 && !bestCap.empty()) {pick = info.PickRandom(bestCap); type = TASKTYPE::CAPTURE; }
                    else if(round < 4 && !bestAid.empty()) {pick = info.PickRandom(bestAid); type = TASKTYPE::AID; }
                    else if(round < 5) {pick = hlt::Planet(); type = TASKTYPE::SNIPE; }
                    else {pick = hlt::Planet(); type = TASKTYPE::KILL; }
                    Task task = Task(1, type);
                    task.target = pick;
                    tasks.push_back(task);
                    round++;
                }
                break;
            }
            case STAGE::DESTROY:{
                hlt::Vector pos = info.CenterOfGravity(info.ourPlanets);
                hlt::Planet target = info.ClosestPlanet(info.enemyPlanets, pos);
                hlt::Planet capTarget = info.ClosestPlanet(info.emptyPlanets, pos);
                for(int i = 0; i < coins; i++) {
                    if(round < 2 && !info.IsNull(capTarget)){
                        Task task = Task(1, TASKTYPE::CAPTURE);
                        task.target = capTarget;
                        tasks.push_back(task);
                    }else {
                        Task task = Task(1, TASKTYPE::CRASH);
                        task.target = target;
                        tasks.push_back(task);
                    }
                    round++;
                }
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
            if(available[ship.entity_id].active == 9) {
                available[ship.entity_id].ship = ship;
                for(const auto& planet : map.planets){
                    if(planet.entity_id == available[ship.entity_id].target.entity_id)
                        available[ship.entity_id].target = planet;
                }
            }
        }
    }
    void Commander::CommandShip(hlt::Ship& ship, Task task) {
        if(available[ship.entity_id].active == 9) return;
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
    }
}