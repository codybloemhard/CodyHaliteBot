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
        if(round > 128)  round = 0;
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
                    if(!info.IsNull(ship.second.target) && !ship.second.target.owned){
                        if(FlyToPlanetRandom(map, _ship, ship.second.target, moves)) continue;
                        else FreeShip(ship.first);
                    }
                    hlt::Planet target0 = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet target1 = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet target = info.OptimalPlanet(_ship.pos, target0, target1, 1, 1);
                    FlyToPlanet(map, _ship, target, moves);
                    break;
                }
                case TASKTYPE::AID:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    if(!info.IsNull(ship.second.target) && ship.second.target.owner_id == info.GetPlayerID()){
                        if(FlyToPlanet(map, _ship, ship.second.target, moves)) continue;
                        else FreeShip(ship.first);
                    }
                    hlt::Planet target0 = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet target1 = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet target = info.OptimalPlanet(_ship.pos, target0, target1, 1, 1);
                    FlyToPlanet(map, _ship, target, moves);
                    break;
                }
                case TASKTYPE::KILL:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    hlt::Planet target;
                    /*target = ship.second.target;
                    if(!info.IsNull(target)){
                        hlt::Ship victim = info.ShipOnPlanet(target);
                        if(info.IsNull(victim)){
                            FreeShip(ship.first);
                            continue;
                        }else{
                            if(FlyToShip(map, _ship, victim, moves)) continue;
                            else FreeShip(ship.first);
                        }
                    }*/
                    hlt::Planet bestKill = info.OptimalPlanet(info.enemyPlanets, _ship.pos);
                    hlt::Planet bestAid = info.OptimalPlanet(info.notfullPlanets, _ship.pos);
                    hlt::Planet bestCap = info.OptimalPlanet(info.emptyPlanets, _ship.pos);
                    hlt::Planet best = info.OptimalPlanet(_ship.pos, bestAid, bestCap, 1, 1);
                    target = info.OptimalPlanet(_ship.pos, bestKill, best, 1, 1);
                    if(target.owner_id == info.GetPlayerID() || !target.owned)
                        FlyToPlanet(map, _ship, target, moves);
                    else{
                        hlt::Ship victim = info.ShipOnPlanet(target);
                        FlyToShip(map, _ship, victim, moves);
                    }
                    break;
                }
                case TASKTYPE::SNIPE:{
                    hlt::Ship victim = info.ClosestEnemyOnPlanet(_ship.pos);
                    if(info.IsNull(victim)) victim = info.ClosestAttacker(_ship.pos);
                    if(info.IsNull(victim)) victim = info.ClosestEnemy(_ship.pos);
                    FlyToShip(map, _ship, victim, moves);
                    break;
                }
                case TASKTYPE::CRASH:{
                    if(ControlDocking(_ship, info, moves)) continue;
                    hlt::Planet target = info.ClosestPlanet(info.notfullPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 35 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    target = info.ClosestPlanet(info.emptyPlanets, _ship.pos);
                    if(_ship.pos.dist(target.pos) < 35 && !info.IsNull(target)){
                        if(FlyToPlanet(map, _ship, target, moves)) continue;
                    }
                    if(!info.IsNull(ship.second.target)){
                        FlyToSuicide(map, _ship, ship.second.target, moves);
                        continue;
                    }
                    target = info.OptimalPlanet(info.enemyPlanets, _ship.pos);
                    FlyToSuicide(map, _ship, target, moves);
                    break;
                }
                case TASKTYPE::DEFEND:{
                    hlt::Ship victim = info.ClosestAttacker(_ship.pos);
                    if(info.IsNull(victim)) victim = info.ClosestEnemyOnPlanet(_ship.pos);
                    if(info.IsNull(victim)) victim = info.ClosestEnemy(_ship.pos);
                    FlyToShip(map, _ship, victim, moves);
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
                        int j = round % 2;
                        hlt::Planet pick = best[j];
                        Task task = Task(1, TASKTYPE::CAPTURE);
                        task.target = pick;
                        tasks.push_back(task);
                        round++;
                    }
                }
                break;
            }
            case STAGE::SETTLE:{
                for(int i = 0; i < coins; i++) {
                    hlt::Planet pick = hlt::Planet();
                    TASKTYPE type;
                    if(round % 5 == 0) { type = TASKTYPE::SNIPE; }
                    else{ type = TASKTYPE::KILL; }
                    Task task = Task(1, type);
                    task.target = pick;
                    tasks.push_back(task);
                    round++;
                }
            }
            case STAGE::CONQUER:{
                for(int i = 0; i < coins; i++){
                    hlt::Planet pick = hlt::Planet();
                    TASKTYPE type;
                    if(round % 5 == 0) { type = TASKTYPE::CAPTURE; }
                    else if(round % 3 == 0){
                        int r = rand() * 1000;
                        if(r < 500) type = TASKTYPE::SNIPE;
                        else type = TASKTYPE::DEFEND;
                    }
                    else { type = TASKTYPE::KILL; }
                    Task task = Task(1, type);
                    task.target = pick;
                    tasks.push_back(task);
                    round++;
                }
                break;
            }
            case STAGE::DESTROY:{
                for(int i = 0; i < coins; i++) {
                    TASKTYPE type;
                    if(round % 5 == 0) type = TASKTYPE::CAPTURE;
                    else type = TASKTYPE::CRASH;
                    Task task = Task(1, type);
                    task.target = hlt::Planet();
                    tasks.push_back(task);
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