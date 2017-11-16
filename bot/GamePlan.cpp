//
// Created by Cody on 10/11/2017.
//

#include "GamePlan.h"

namespace bot{
    STAGE GamePlan::GetStage() {
        return stage;
    }
    void GamePlan::Step(hlt::Map& map, Information &info) {
        if(stage == STAGE::START && info.ourPlanets.size() >= 2)
            stage = STAGE::SETTLE;
        if(stage == STAGE::SETTLE && info.notfullPlanets.size() == 0)
            stage = STAGE::CONQUER;
        if(stage == STAGE::CONQUER && info.PowerPart(map) > 0.6)
            stage = STAGE::DESTROY;
        if(stage == STAGE::DESTROY && info.PowerPart(map) < 0.55)
            stage = STAGE::CONQUER;
    }
}