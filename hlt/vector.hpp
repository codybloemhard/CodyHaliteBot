#pragma once

#include <ostream>

#include "constants.hpp"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace hlt {
    struct Vector {
        double x, y;

        Vector() : x(0), y(0) {};

        Vector(double x, double y);

        double dist(const Vector &target) const;

        double dist_sq(const Vector &target) const;

        Vector closest_point(const Vector &center, double radius) const;

        std::pair<Vector, Vector> tangents(const Vector &center, double radius) const;

        double angle() const;

        double angle_between(Vector v) const;

        double length() const;

        Vector normalize() const;

        double dist_line(const Vector &a, const Vector &b) const;

        double dot_prod(const Vector &v) const;

        double cross_prod(const Vector &v) const;

        Vector operator-(const Vector &v) const;

        Vector operator+(const Vector &v) const;

        Vector operator/(double u) const;

        Vector operator*(double u) const;

        friend std::ostream &operator<<(std::ostream &out, const Vector &location);
    };

    static bool operator==(const Vector &l1, const Vector &l2) {
        return l1.x == l2.x && l1.y == l2.y;
    }

    double rad_to_deg(double rad);

    double deg_to_rad(double deg);
}
