//
// Created by Cody on 11/11/2017.
//

#ifndef MYBOT_COMMANDER_H
#define MYBOT_COMMANDER_H

#include <vector>
#include "Information.h"
#include "GamePlan.h"
#include "ShipControl.h"
#include <utility>

namespace bot {
    enum TASKTYPE { NOOP, CAPTURE, AID, KILL };
    struct Task{
    public:
        unsigned int active = 0;
        TASKTYPE type = NOOP;
        hlt::Ship ship;
        Task() : active(0), type(TASKTYPE::NOOP), ship(hlt::Ship()){}
        Task(unsigned int act, TASKTYPE typ, const hlt::Ship& shi) : active(act), type(typ), ship(shi){}
    };
    class Commander {
    private:
        hlt::Planet globalTarget;
        std::unordered_map<unsigned int, Task> available;
    private:
        void ControlShips(hlt::Map&, std::vector<hlt::Move>&, Information&);
        void CommandShip(const hlt::Ship&, TASKTYPE);
        bool ControlDocking(const hlt::Ship&, Information&, std::vector<hlt::Move>&);
        void FreeShip(const unsigned int);
    public:
        void Command(hlt::Map&, std::vector<hlt::Move>&, Information&, GamePlan&);
    };
}

#endif //MYBOT_COMMANDER_H