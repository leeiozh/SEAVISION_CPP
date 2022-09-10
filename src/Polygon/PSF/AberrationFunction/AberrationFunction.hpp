//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_ABERRATIONFUNCTION_HPP
#define POLYGON_ABERRATIONFUNCTION_HPP

#include <eigen3/Eigen/Dense>

namespace Polygon {

class AberrationFunction {
    /**
     * класс функции аберрации
     */

protected:
    Eigen::MatrixXd c_coeffs; // коэффициенты разложения функции по косинусам
    Eigen::MatrixXd s_coeffs; // коэффициенты разложения функции по синусам

public:

    /**
     * конструктор класса по коэффициентам разложения функции
     * @param c_coeffs коэффициенты разложения функции по косинусам
     * @param s_coeffs коэффициенты разложения функции по синусам
     */
    AberrationFunction(const Eigen::MatrixXd &c_coeffs, const Eigen::MatrixXd &s_coeffs);

    /**
     * вычисление полиномов Цернице
     * @param x_pow массив х по степеням
     * @param y_pow массив у по степеням
     * @param n количество исчисляемых гармоник
     * @param m количество исчисляемых гармоник
     * @return
     */
    [[nodiscard]] static std::pair<double, double>
    calc_zernike_polynom(const Eigen::VectorXd &x_pow, const Eigen::VectorXd &y_pow, const int &n, const int &m);

    /**
     * вычисление функции аберрации в точке
     * @param x координата х точки в предметной плоскости
     * @param y координата у точки в предметной плоскости
     * @return функция аберрации в точке
     */
    [[nodiscard]] double calc_aberration_func(const double &x, const double &y) const;

};
}

#endif //POLYGON_ABERRATIONFUNCTION_HPP
