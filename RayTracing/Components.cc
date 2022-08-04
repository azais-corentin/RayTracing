#include "Components.hh"
#include "HitRecord.hh"

namespace RayTracing::Components {

std::optional<HitRecord> Sphere::hit(const Ray &ray, double tMinimum, double tMaximum) const {
    const Vector3 oc = (ray.origin() - center);

    const double a      = ray.direction().squaredNorm();
    const double half_b = oc.dot(ray.direction());
    const double c      = oc.squaredNorm() - radius * radius;

    const double discriminant = half_b * half_b - a * c;

    if (discriminant < 0) { return std::nullopt; }

    const double sqrtd = std::sqrt(discriminant);

    /*
    auto root = (-half_b - sqrtd) / a;
    if (root < tMinimum or tMaximum < root) {
        root = (-half_b + sqrtd) / a;
        if (root < tMinimum or tMaximum < root) { return std::nullopt; }
    }

    const auto point = ray.at(root);
    return HitRecord{ray, (point - center) / radius, point, root};
    */

    // Find the nearest root that lies in the acceptable range

    // Test smallest root
    {
        const double root = (-half_b - sqrtd) / a;

        if (root >= tMinimum and root <= tMaximum) {
            const Point3 point = ray.at(root);
            return HitRecord{ray, (point - center) / radius, point, root};
        }
    }
    // Test biggest root
    {
        const double root = (-half_b + sqrtd) / a;
        if (root >= tMinimum and root <= tMaximum) {
            const Point3 point = ray.at(root);
            return HitRecord{ray, (point - center) / radius, point, root};
        }
    }

    // No hit
    return std::nullopt;
}
} // namespace RayTracing::Components
