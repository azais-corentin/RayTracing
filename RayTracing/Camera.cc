#include "Camera.hh"

namespace RayTracing {

Camera::Camera(double aspectRatio, double focalLength)
    : origin_{0., 0., 0.}, horizontal_{2.0 * aspectRatio, 0.0, 0.0}, vertical_{0.0, 2.0, 0.0},
      lowerLeftCorner_{origin_ - horizontal_ / 2. - vertical_ / 2. - Vector3{0., 0., focalLength}} {}

Ray Camera::getRay(double u, double v) const {
    return {origin_, lowerLeftCorner_ + u * horizontal_ + v * vertical_ - origin_};
}

} // namespace RayTracing
