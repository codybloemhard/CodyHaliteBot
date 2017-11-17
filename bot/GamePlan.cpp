//
// Created by Cody on 10/11/2017.
//

#include "GamePlan.h"

namespace bot{
    bool GamePlan::DidChange(){
        return didChange;
    }
    STAGE GamePlan::GetStage() {
        return stage;
    }
    void GamePlan::Step(hlt::Map& map, Information &info) {
        didChange = false;
        if(stage == STAGE::START && info.ourPlanets.size() >= 2) {
            stage = STAGE::CONQUER;
            didChange = true;
        }
        if(stage == STAGE::CONQUER && info.PowerPart(map) > 0.6) {
            stage = STAGE::DESTROY;
            didChange = true;
        }
        if(stage == STAGE::DESTROY && info.PowerPart(map) < 0.55) {
            stage = STAGE::CONQUER;
            didChange = true;
        }
    }
}