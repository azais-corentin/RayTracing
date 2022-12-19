#include <RayTracing/HitRecord.hh>

namespace RayTracing {

HitRecord::HitRecord(const Ray &ray, const Vector3 &outwardNormal, Point3 point, double t)
    : point_{point}, t_{t}, frontFace_{ray.direction().dot(outwardNormal) < 0}, //
      normal_{frontFace_ ? outwardNormal : -outwardNormal} {}

} // namespace RayTracing
