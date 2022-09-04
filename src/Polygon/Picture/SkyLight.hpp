//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_SKYLIGHT_HPP
#define POLYGON_SKYLIGHT_HPP

#include "../Utility/Consts.hpp"
#include "../Utility/Structs.hpp"
#include "../VisualField/BaseVisualField.hpp"
#include <cmath>
#include <memory>

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class SkyLight {
protected:
    double zenith_bright = 0;
public:
    inline explicit SkyLight(const double &zenith_bright = 0.) : zenith_bright(zenith_bright) {}

    [[nodiscard]] inline double calc_at_point(const double &zenith) const {
        double sinz2 = std::sin(zenith) * std::sin(zenith);
        double ten_kx_z = std::pow(10, -0.4 * K_ATMOSPHERE / std::sqrt(1 - 0.96 * sinz2));
        return zenith_bright * ten_kx_z * (1 - ten_kx_z);
    }

    [[nodiscard]] inline Eigen::MatrixXd
    calc_background(const ScopeState &scope, const std::shared_ptr<BaseVisualField> &field) const {
        double az = std::atan2(scope.position.y(), scope.position.x());
        double phi = std::acos(scope.position.z() / scope.position.norm());
        double ra = std::atan2(scope.direction.y(), scope.direction.x());
        double dec = std::acos(scope.direction.z() / scope.direction.norm());
        double time = az - ra;
        double zenith = std::acos(std::sin(phi) * std::sin(dec) + std::cos(phi) * std::cos(dec) * std::cos(time));

        Eigen::MatrixXd res = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);
        for (int i = 0; i < SIZE_X; ++i) {
            double back = calc_at_point(
                    zenith + field->get_cone_angle() * (static_cast<double>(i) / static_cast<double>(SIZE_X) - 0.5));
            for (int j = 0; j < SIZE_Y; ++j) {
                res(i, j) = back;
            }
        }
        return res;
    }

};

}

#endif //POLYGON_SKYLIGHT_HPP
