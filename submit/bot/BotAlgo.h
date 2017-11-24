//
// Created by Cody on 09/11/2017.
//
#ifndef MYBOT_BOTALGO_H
#define MYBOT_BOTALGO_H

#include "../hlt/types.hpp"
#include "../hlt/navigation.hpp"
#include "Information.h"
#include "GamePlan.h"
#include "Commander.h"

namespace bot{
    class BotAlgo{
    private:
        Information info;
        GamePlan plan;
        Commander commander;
    public:
        BotAlgo(hlt::PlayerId id) : info(Information(id)), plan(GamePlan()){ }
        void Moves(hlt::Map& map, std::vector<hlt::Move>& moves){
            info.SortPlanets(map);
            info.SortEnemyShips(map);
            info.RecordShipSpeed(map);
            commander.Command(map, moves, info, plan);
            info.EndCycle();
            plan.Step(map, info);
        }
    };
}

#endif //MYBOT_BOTALGO_H