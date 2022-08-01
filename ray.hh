#pragma once

#include <Eigen/Dense>

namespace RayTracing {

using Vector3 = Eigen::Vector3d;
using Point3  = Eigen::Vector3d;
using Color   = Eigen::Vector3d;

class Ray {
  public:
    Ray() = default;
    Ray(const Point3 &origin, const Vector3 &direction);

    Point3 origin() const;
    Vector3 direction() const;

    Eigen::Vector3d at(double t) const;

  private:
    Point3 origin_;
    Vector3 direction_;
};

} // namespace RayTracing
