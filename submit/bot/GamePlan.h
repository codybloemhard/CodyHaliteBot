//
// Created by Cody on 10/11/2017.
//

#ifndef MYBOT_GAMEPLAN_H
#define MYBOT_GAMEPLAN_H

#include "../hlt/map.hpp"
#include "Information.h"

namespace bot {
    enum STAGE {
        START, SETTLE, CONQUER, DESTROY
    };

    class GamePlan {
    private:
        STAGE stage;
        bool didChange;
    public:
        bool DidChange();
        STAGE GetStage();
        void Step(hlt::Map&, Information&);
    };
}
#endif //MYBOT_GAMEPLAN_H
