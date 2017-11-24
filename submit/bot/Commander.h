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
    enum TASKTYPE { NOOP, CAPTURE, AID, KILL, SNIPE, CRASH, DEFEND };
    struct Task{
    public:
        int active = 0;
        TASKTYPE type = NOOP;
        hlt::Ship ship;
        hlt::Planet target;
        Task() : active(0), type(TASKTYPE::NOOP), ship(hlt::Ship()), target(hlt::Planet()){}
        Task(int act, TASKTYPE typ) : active(act), type(typ){
            target = hlt::Planet();
        }
    };
    class Commander {
    private:
        std::unordered_map<unsigned int, Task> available;
        std::vector<Task> tasks;
        unsigned int round = 0;
    private:
        void ControlShips(hlt::Map&, std::vector<hlt::Move>&, Information&);
        void CommandShip(hlt::Ship&, Task);
        bool ControlDocking(const hlt::Ship&, Information&, std::vector<hlt::Move>&);
        void FreeShip(unsigned int);
        void GenerateTasks(hlt::Map&, Information&, GamePlan&);
        void DelegateTasks(Information&, GamePlan&);
        void UpdateShips(hlt::Map&, hlt::PlayerId);
    public:
        void Command(hlt::Map&, std::vector<hlt::Move>&, Information&, GamePlan&);
    };
}

#endif //MYBOT_COMMANDER_H