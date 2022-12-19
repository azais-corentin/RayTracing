#pragma once

#include "Ray.hh"

#include <optional>

namespace RayTracing {

struct HitRecord;

namespace Components {

struct Sphere {
    Point3 center;
    double radius;

    std::optional<HitRecord> hit(const Ray &ray, double tMinimum, double tMaximum) const;
};

} // namespace Components

} // namespace RayTracing
