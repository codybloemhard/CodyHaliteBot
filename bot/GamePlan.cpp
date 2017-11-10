//
// Created by Cody on 10/11/2017.
//

#include "GamePlan.h"

namespace bot{
    STAGE GamePlan::GetStage() {
        return stage;
    }

    void GamePlan::Step(hlt::Map &map, Information &info) {
        if(info.ourPlanets.size() >= 3 && stage == STAGE::START)
            stage = STAGE::SETTLE;
        if(info.notfullPlanets.size() == 0 && stage == STAGE::SETTLE)
            stage = STAGE::CONQUER;
    }
}