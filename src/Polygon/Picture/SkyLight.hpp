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
    /**
     * класс фона неба
     */
protected:
    double zenith_bright = 0; // зенитная яркость неба (в единицах яркости (интенсивности))
public:
    /**
     * конструктор класса фона неба
     * @param zenith_bright зенитная яркость
     */
    inline explicit SkyLight(const double &zenith_bright = 0.) : zenith_bright(zenith_bright) {}

    /**
     * вычисление фона неба в точке с поданным зенитным расстоянием
     * @param zenith зенитное расстояние
     * @return яркость неба в поданной точке
     */
    [[nodiscard]] inline double calc_at_point(const double &zenith) const {
        double sinz2 = std::sin(zenith) * std::sin(zenith);
        double ten_kx_z = std::pow(10, -0.4 * K_ATMOSPHERE / std::sqrt(1 - 0.96 * sinz2));
        return zenith_bright * ten_kx_z * (1 - ten_kx_z);
    }

    /**
     * вычисление матрицы фона неба
     * @param scope структура с положением и ориентацией телескопа
     * @param field класс поля зрения
     * @return матрица со значениями фона неб в каждой точке предметной плоскости
     */
    [[nodiscard]] inline Eigen::MatrixXd
    calc_background(const ScopeState &scope, const std::shared_ptr<BaseVisualField> &field) const {
        double az = std::atan2(scope.position.y(), scope.position.x()); // азимут точки
        double phi = std::acos(scope.position.z() / scope.position.norm()); // широта точки
        double ra = std::atan2(scope.direction.y(), scope.direction.x()); // прямое восхождение исследуемой точки неба
        double dec = std::acos(scope.direction.z() / scope.direction.norm()); // склонение исследуемой точки неба
        double time = az - ra; // часовой угол
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
