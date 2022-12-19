#include <RayTracing/Random.hh>

#include <boost/ut.hpp>

#include <array>
#include <numeric>

int main() {
    using namespace boost::ut;
    using namespace RayTracing;

    "RandomDouble"_test = [] {
        std::array<double, 10000> rd;
        RandomDouble rdgen(0.0, 1.0);

        std::generate(rd.begin(), rd.end(), [&] { return rdgen(); });

        const auto mean = std::reduce(rd.begin(), rd.end()) / static_cast<double>(rd.size());

        expect(mean == 0.50_d) << "average is 0.5 +- 0.01";

        const auto [itmin, itmax] = std::minmax_element(rd.begin(), rd.end());
        const auto [min, max]     = std::make_pair(*itmin, *itmax);
        expect(min <= max) << "min <= max";
        expect(min >= 0.0_d) << "min >= 0";
        expect(min <= 1.0_d) << "min <= 1";
        expect(max <= 1.0_d) << "max <= 1";
        expect(max >= 0.0_d) << "max >= 0";
    };

    "RandomVector"_test = [] {
        std::array<Vector3, 10000> rd;
        RandomVector rdgen(-1.0, 1.0);

        std::generate(rd.begin(), rd.end(), [&] { return rdgen(); });

        const auto mean = std::reduce(rd.begin(), rd.end()).sum() / (3 * rd.size());

        expect(mean == 0.00_d) << "average is 0 +- 0.01";

        for (auto &v : rd) {
            const auto min = v.minCoeff();
            const auto max = v.maxCoeff();
            expect(min <= max) << "min <= max";
            expect(min >= -1.0_d) << "min >= -1";
            expect(min <= 1.0_d) << "min <= 1";
            expect(max <= 1.0_d) << "max <= 1";
            expect(max >= -1.0_d) << "max >= -1";
        }
    };

    "randomVectorInSphere"_test = [] {
        std::array<Vector3, 10000> rd;
        RandomVector rdgen(-1.0, 1.0);

        std::generate(rd.begin(), rd.end(), [&] { return randomVectorInSphere(rdgen); });

        double average_norm = 0.0;
        Vector3 average     = Vector3::Zero();
        for (auto &v : rd) {
            average += v;

            expect(v.norm() <= 1.0_d);
            expect(v.norm() > 0.0_d);
        }
        average /= rd.size();
        expect(average.x() == 0.0_d);
        expect(average.y() == 0.0_d);
        expect(average.z() == 0.0_d);
    };
}
