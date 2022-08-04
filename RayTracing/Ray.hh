#pragma once

#include <Eigen/Dense>

namespace RayTracing {

using Vector3 = Eigen::Vector3d;
using Point3  = Eigen::Vector3d;
using Color   = Eigen::Vector3d;

struct Ray {
    Ray(const Point3 &origin, const Vector3 &direction);

    const Point3 &origin() const;
    Point3 &origin();

    const Vector3 &direction() const;
    Vector3 &direction();

    Eigen::Vector3d at(double t) const;

  private:
    Point3 origin_;
    Vector3 direction_;
};

} // namespace RayTracing
