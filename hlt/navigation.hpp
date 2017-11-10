#pragma once

#include <cmath>
#include "collision.hpp"
#include "map.hpp"
#include "move.hpp"

namespace hlt {
    namespace navigation {
        static void check_and_add_entity_between(
                std::vector<const Entity *>& entities_found,
                const Vector& start,
                const Vector& target,
                const Entity& entity_to_check)
        {
            const Vector &location = entity_to_check.pos;
            if (location == start || location == target) {
                return;
            }
            if (collision::segment_circle_intersect(start, target, entity_to_check, constants::FORECAST_FUDGE_FACTOR)) {
                entities_found.push_back(&entity_to_check);
            }
        }

        static std::vector<const Entity *> objects_between(const Map& map, const Vector& start, const Vector& target) {
            std::vector<const Entity *> entities_found;

            for (const Planet& planet : map.planets) {
                check_and_add_entity_between(entities_found, start, target, planet);
            }

            for (const auto& player_ship : map.ships) {
                for (const Ship& ship : player_ship.second) {
                    check_and_add_entity_between(entities_found, start, target, ship);
                }
            }

            return entities_found;
        }

        static nullable<Move> navigate_ship_towards_target(
                const Map& map,
                const Ship& ship,
                const Vector& target,
                const int max_thrust,
                const bool avoid_obstacles,
                const int max_corrections,
                const double angular_step_rad)
        {
            if (max_corrections <= 0) {
                return { Move::noop(), false };
            }

            const double distance = ship.pos.dist(target);
            const double angle = ship.pos.angle_between(target);

            if (avoid_obstacles && !objects_between(map, ship.pos, target).empty()) {
                const double new_target_dx = std::cos(angle + angular_step_rad) * distance;
                const double new_target_dy = std::sin(angle + angular_step_rad) * distance;
                const Vector new_target = { ship.pos.x + new_target_dx, ship.pos.y + new_target_dy };

                return navigate_ship_towards_target(
                        map, ship, new_target, max_thrust, true, (max_corrections - 1), angular_step_rad);
            }

            int thrust;
            if (distance < max_thrust) {
                // Do not round up, since overshooting might cause collision.
                thrust = (int) distance;
            } else {
                thrust = max_thrust;
            }

            return {Move::thrust(ship.entity_id, thrust, static_cast<const int>(rad_to_deg(angle))), true };
        }

        static nullable<Move> navigate_ship_to_dock(
                const Map& map,
                const Ship& ship,
                const Entity& dock_target,
                const int max_thrust)
        {
            const int max_corrections = constants::MAX_NAVIGATION_CORRECTIONS;
            const bool avoid_obstacles = true;
            const double angular_step_rad = M_PI / 180.0;
            const Vector& target = ship.pos.closest_point(dock_target.pos, dock_target.radius);

            return navigate_ship_towards_target(
                    map, ship, target, max_thrust, avoid_obstacles, max_corrections, angular_step_rad);
        }
    }
}
