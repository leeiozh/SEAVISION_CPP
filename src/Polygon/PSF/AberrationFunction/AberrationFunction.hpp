//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_ABERRATIONFUNCTION_HPP
#define POLYGON_ABERRATIONFUNCTION_HPP

#include <eigen3/Eigen/Dense>
#include "../../PoligonBaseException.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class AberrationFunction {
    /**
     * класс функции аберрации
     */

protected:
    Eigen::MatrixXd c_coeffs; // коэффициенты разложения функции по косинусам
    Eigen::MatrixXd s_coeffs; // коэффициенты разложения функции по синусам
    Eigen::MatrixXd x_pow = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);
    Eigen::MatrixXd y_pow = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y); // пара матриц с сетками степеней координат

public:

    /**
     * конструктор класса по коэффициентам разложения функции
     * @param c_coeffs коэффициенты разложения функции по косинусам
     * @param s_coeffs коэффициенты разложения функции по синусам
     */
    AberrationFunction(const Eigen::MatrixXd &c_coeffs, const Eigen::MatrixXd &s_coeffs) {

        if (c_coeffs.cols() != s_coeffs.cols() || c_coeffs.rows() != s_coeffs.rows()) {
            std::stringstream buff;
            buff << "Размеры матриц коэффициентов не совпадают! Размер матрицы C: "
                 << c_coeffs.cols() << " " << c_coeffs.rows() << ". Размер матрицы S: " <<
                 s_coeffs.cols() << " " << s_coeffs.rows() << std::endl;
            throw PolygonBaseException(buff.str().c_str());
        }

        for (int i = 0; i < SIZE_X; ++i) {
            x_pow(i, 0) = 4 * static_cast<double>(i) / (SIZE_X - 1) - 2;
        }

        for (int j = 0; j < SIZE_Y; ++j) {
            y_pow(j, 0) = 4 * static_cast<double>(j) / (SIZE_Y - 1) - 2;
        }

        for (int i = 1; i < c_coeffs.rows(); ++i) {
            for (int j = 0; j < SIZE_X; ++j) {
                x_pow(j, i) = x_pow(j, i - 1) * x_pow(j, 0);
            }
            for (int j = 0; j < SIZE_Y; ++j) {
                y_pow(j, i) = y_pow(j, i - 1) * y_pow(j, 0);
            }
        }
    }

    /**
     * вычисление полиномов Цернице
     * @param x_pow массив х по степеням
     * @param y_pow массив у по степеням
     * @param n количество исчисляемых гармоник
     * @param m количество исчисляемых гармоник
     * @return
     */
    [[nodiscard]] std::pair<double, double>
    calc_zernike_polynom(const int &i, const int &j, const int &n, const int &m) const {
        switch (n) {
            case 0:
                return std::make_pair(1, 0);
            case 1:
                return std::make_pair(y_pow(j, 0), x_pow(i, 0));
            case 2:
                switch (m) {
                    case 0:
                        return std::make_pair(-1 + 2 * (x_pow(i, 1), y_pow(j, 1)), 0);
                    case 1:
                        return std::make_pair(-x_pow(i, 1) + y_pow(j, 1), 2 * x_pow(i, 0) * y_pow(j, 0));
                    default:
                        return std::make_pair(0, 0);
                }
            case 3:
                switch (m) {
                    case 1:
                        return std::make_pair(-2 * y_pow(j, 0) + 3 * y_pow(j, 2) + 3 * x_pow(i, 1) * y_pow(j, 0),
                                              -2 * x_pow(i, 0) + 3 * x_pow(i, 2) + 3 * x_pow(i, 0) * y_pow(j, 1));
                    case 3:
                        return std::make_pair(y_pow(j, 2) - 3 * x_pow(i, 1) * y_pow(j, 0),
                                              -x_pow(i, 2) + 3 * x_pow(i, 0) * y_pow(j, 1));
                    default:
                        return std::make_pair(0, 0);
                }
            case 4:
                switch (m) {
                    case 0:
                        return std::make_pair(
                                1 - 6 * x_pow(i, 1) - 6 * y_pow(j, 1) + 6 * x_pow(i, 3) +
                                12 * x_pow(i, 1) * y_pow(j, 1) + 6 * y_pow(j, 3), 0);
                    case 2:
                        return std::make_pair(3 * x_pow(i, 1) - 3 * y_pow(j, 1) - 4 * x_pow(i, 3) + 4 * y_pow(j, 3),
                                              -6 * x_pow(i, 0) * y_pow(j, 0) + 8 * x_pow(i, 2) * y_pow(j, 0) +
                                              8 * x_pow(i, 0) * y_pow(j, 2));
                    case 4 :
                        return std::make_pair(x_pow(i, 3) - 6 * x_pow(i, 1) * y_pow(j, 1) + y_pow(j, 3),
                                              -4 * x_pow(i, 2) * y_pow(j, 0) + 4 * x_pow(i, 0) * y_pow(j, 2));
                    default:
                        return std::make_pair(0, 0);
                }
            default:
                return std::make_pair(0, 0);
        }
    }

    /**
     * вычисление функции аберрации в точке
     * @param i индекс координаты х точки в предметной плоскости
     * @param j индекс координаты у точки в предметной плоскости
     * @return функция аберрации в точке
     */
    [[nodiscard]] double calc_aberration_func(const int &i, const int &j) const {
        double res = 0;

        // суммирование гармоник разложения
        for (int n = 0; n < c_coeffs.rows(); ++n) {
            for (int m = 0; m < c_coeffs.cols(); ++m) {
                auto pair = calc_zernike_polynom(i, j, n, m);
                res += c_coeffs(n, m) * pair.first + s_coeffs(n, m) * pair.second;
            }
        }

        return res;
    }

};
}

#endif //POLYGON_ABERRATIONFUNCTION_HPP
