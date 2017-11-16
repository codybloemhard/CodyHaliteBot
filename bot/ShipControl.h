//
// Created by Cody on 11/11/2017.
//

#ifndef MYBOT_SHIPCONTROL_H
#define MYBOT_SHIPCONTROL_H

#include "../hlt/entities.hpp"
#include "../hlt/move.hpp"
#include "../hlt/navigation.hpp"

namespace bot{
    bool IsDocking(const hlt::Ship& ship);
    bool FlyToDock(const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves);
    bool FlyToPlanet(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves);
    bool FlyToPlanetRandom(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves);
    bool FlyToShip(const hlt::Map& map, const hlt::Ship& ship, const hlt::Ship& target, std::vector<hlt::Move>& moves);
    bool FlyToSuicide(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& target, std::vector<hlt::Move>& moves);
}

#endif //MYBOT_SHIPCONTROL_H