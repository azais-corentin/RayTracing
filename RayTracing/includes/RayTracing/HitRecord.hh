#pragma once

#include "Ray.hh"

namespace RayTracing {

struct HitRecord {
    /*!
     * \brief Creates a HitRecord and compute the normal
     */
    HitRecord(const Ray &ray, const Vector3 &outwardNormal, Point3 point, double t);

    Point3 point_;
    double t_;
    bool frontFace_;
    Vector3 normal_;
};
} // namespace RayTracing
