#include "ray.hh"

namespace RayTracing {

Ray::Ray(const Point3 &origin, const Vector3 &direction) : origin_{origin}, direction_{direction} {}

Point3 Ray::origin() const { return origin_; }

Vector3 Ray::direction() const { return direction_; }

Eigen::Vector3d Ray::at(double t) const { return origin_ + t * direction_; }

} // namespace RayTracing
