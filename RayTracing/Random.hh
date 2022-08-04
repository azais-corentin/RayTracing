#pragma once

#include "RayTracing/Ray.hh"

#include <EigenRand/EigenRand>

#include <random>

namespace RayTracing {

/*!
 * \brief The RandomDouble class generates random doubles in range [0, 1]
 *
 * \warning This class is not threadsafe!
 */
struct RandomDouble {
    double operator()() { return distribution_(generator_); }

  private:
    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<double> distribution_{0.0, 1.0};
};

/*!
 * \brief The RandomVector class generates random vectors.
 *
 * \warning This class is not threadsafe!
 */
struct RandomVector {
    RandomVector(double min, double max) /*: distribution{min, max}*/ {}

    Vector3 operator()() {
        constexpr auto rnd = [] { return -1.0 + rand() / (RAND_MAX / 2.0); };

        return Vector3{rnd(), rnd(), rnd()};

        // return distribution.generateLike(Vector3{}, generator_);
    }

  private:
    // Eigen::Rand::P8_mt19937_64 generator_{42}; // std::random_device{}()
    // Eigen::Rand::UniformRealGen<double> distribution;
};

inline Vector3 randomVectorInSphere(RandomVector &generator, double radius = 1.0) {
    while (true) {
        const auto vector = generator();
        if (vector.squaredNorm() < radius) { return vector; }
    }
}

} // namespace RayTracing
