//
// Created by Cody on 12/11/2017.
//
#include "ShipControl.h"
#include "../hlt/log.hpp"

namespace bot{
    bool IsDocking(const hlt::Ship& ship){
        return ship.docking_status == hlt::ShipDockingStatus::Docking;
    }
    bool FlyToDock(const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves){
        if(ship.can_dock(planet) && !planet.is_full() && (!planet.owned || planet.owner_id == ship.owner_id)) {
            moves.push_back(hlt::Move::dock(ship.entity_id, planet.entity_id));
            return true;
        }
        return false;
    }
    bool FlyToPlanet(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves){
        const hlt::nullable<hlt::Move> move = hlt::navigation::navigate_ship_to_dock(map, ship, planet, 7);
        if (move.second) {
            moves.push_back(move.first);
            return true;
        }
        return false;
    }
    bool FlyToPlanetRandom(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& planet, std::vector<hlt::Move>& moves){
        double angle = (double)(rand() % 1000) / 1000.0 * M_PI * 2;
        double x = (cos(angle) * planet.radius) + planet.pos.x;
        double y = (sin(angle) * planet.radius) + planet.pos.y;
        const int max_corrections = hlt::constants::MAX_NAVIGATION_CORRECTIONS;
        const bool avoid_obstacles = true;
        const double angular_step_rad = M_PI / 180.0;
        const hlt::Vector target = hlt::Vector(x, y);
        hlt::nullable<hlt::Move> m = hlt::navigation::navigate_ship_towards_target(
                map, ship, target, 7, avoid_obstacles, max_corrections, angular_step_rad);
        if(!m.second) return false;
        moves.push_back(m.first);
        return true;
    }
    bool FlyToShip(const hlt::Map& map, const hlt::Ship& ship, const hlt::Ship& target, std::vector<hlt::Move>& moves){
        if(ship.pos.dist(target.pos) < hlt::constants::WEAPON_RADIUS) return true;
        hlt::nullable<hlt::Move> m = hlt::navigation::navigate_ship_towards_target(map, ship, target.pos, 7, true, hlt::constants::MAX_NAVIGATION_CORRECTIONS, M_PI/180);
        if(m.second) {
            moves.push_back(m.first);
            return true;
        }
        return false;
    }
    bool FlyToSuicide(const hlt::Map& map, const hlt::Ship& ship, const hlt::Planet& target, std::vector<hlt::Move>& moves){
        hlt::nullable<hlt::Move> m = hlt::navigation::navigate_ship_towards_target(map, ship, target.pos, 7, true, hlt::constants::MAX_NAVIGATION_CORRECTIONS, M_PI/180);
        if(m.second) {
            moves.push_back(m.first);
            return true;
        }
        return false;
    }
}