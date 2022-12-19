#pragma once

#include "RayTracing/Ray.hh"

#pragma warning(push)
#pragma warning(disable : 4127 4245)
#include <EigenRand/EigenRand>
#pragma warning(pop)

#include <random>

namespace RayTracing {

/*!
 * \brief The RandomDouble class generates random doubles in range [0, 1]
 *
 * \warning This class is not threadsafe!
 */
struct RandomDouble {
    RandomDouble(double min = 0.0, double max = 1.0) : distribution_{min, max} {}

    double operator()() { return distribution_(generator_); }

  private:
    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<double> distribution_;
};

/*!
 * \brief The RandomVector class generates random vectors.
 *
 * \warning This class is not threadsafe!
 */
struct RandomVector {
    RandomVector(double min, double max) : distribution_{min, max} {}

    Vector3 operator()() {
        // return Vector3{distribution_(generator_), distribution_(generator_), distribution_(generator_)};
        return distribution_.generateLike(Vector3{}, generator_);
    }

  private:
    Eigen::Rand::P8_mt19937_64 generator_{std::random_device{}()};
    Eigen::Rand::UniformRealGen<double> distribution_;

    /*
    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<double> distribution_;
    */
};

inline Vector3 randomVectorInSphere(RandomVector &generator) {
    while (true) {
        const Vector3 vector = generator();
        if (vector.squaredNorm() < 1.0) { return vector; }
    }
}

} // namespace RayTracing
