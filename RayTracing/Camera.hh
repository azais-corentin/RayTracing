#pragma once

#include "Ray.hh"

namespace RayTracing {

class Camera {
  public:
    Camera(double aspectRatio = 16.0 / 9.0, double focalLength = 1.0);

    Ray getRay(double u, double v) const;

  private:
    Point3 origin_;
    Vector3 horizontal_, vertical_;
    Point3 lowerLeftCorner_;
};

} // namespace RayTracing
