//
// Created by leeiozh on 06.08.22.
//

#include "AberrationFunction.hpp"
#include "../../PoligonBaseException.hpp"

namespace Polygon {

std::pair<double, double>
AberrationFunction::calc_zernike_polynom(const Eigen::VectorXd &x_pow, const Eigen::VectorXd &y_pow, const int &n,
                                         const int &m) {
    switch (n) {
        case 0:
            return std::make_pair(1, 0);
        case 1:
            return std::make_pair(y_pow[1], x_pow[1]);
        case 2:
            switch (m) {
                case 0:
                    return std::make_pair(-1 + 2 * (x_pow[2], y_pow[2]), 0);
                case 1:
                    return std::make_pair(-x_pow[2] + y_pow[2], 2 * x_pow[1] * y_pow[1]);
                default:
                    return std::make_pair(0, 0);
            }
        case 3:
            switch (m) {
                case 1:
                    return std::make_pair(-2 * y_pow[1] + 3 * y_pow[3] + 3 * x_pow[2] * y_pow[1],
                                          -2 * x_pow[1] + 3 * x_pow[3] + 3 * x_pow[1] * y_pow[2]);
                case 3:
                    return std::make_pair(y_pow[3] - 3 * x_pow[2] * y_pow[1], -x_pow[3] + 3 * x_pow[1] * y_pow[2]);
                default:
                    return std::make_pair(0, 0);
            }
        case 4:
            switch (m) {
                case 0:
                    return std::make_pair(
                            1 - 6 * x_pow[2] - 6 * y_pow[2] + 6 * x_pow[4] + 12 * x_pow[2] * y_pow[2] + 6 * y_pow[4],
                            0);
                case 2:
                    return std::make_pair(3 * x_pow[2] - 3 * y_pow[2] - 4 * x_pow[4] + 4 * y_pow[4],
                                          -6 * x_pow[1] * y_pow[1] + 8 * x_pow[3] * y_pow[1] + 8 * x_pow[1] * y_pow[3]);
                case 4 :
                    return std::make_pair(x_pow[4] - 6 * x_pow[2] * y_pow[2] + y_pow[4],
                                          -4 * x_pow[3] * y_pow[1] + 4 * x_pow[1] * y_pow[3]);
                default:
                    return std::make_pair(0, 0);
            }
        default:
            return std::make_pair(0, 0);
    }
}

AberrationFunction::AberrationFunction(const Eigen::MatrixXd &c_coeffs, const Eigen::MatrixXd &s_coeffs) : c_coeffs(
        c_coeffs), s_coeffs(s_coeffs) {

    if (c_coeffs.cols() != s_coeffs.cols() || c_coeffs.rows() != s_coeffs.rows()) {
        std::stringstream buff;
        buff << "Размеры матриц коэффициентов не совпадают! Размер матрицы C: "
             << c_coeffs.cols() << " " << c_coeffs.rows() << ". Размер матрицы S: " <<
             s_coeffs.cols() << " " << s_coeffs.rows() << std::endl;
        throw PolygonBaseException(buff.str().c_str());
    }
}

double AberrationFunction::calc_aberration_func(const double &x, const double &y) const {

    double res = 0;

    Eigen::VectorXd x_pow = Eigen::VectorXd::Ones(c_coeffs.rows());
    Eigen::VectorXd y_pow = Eigen::VectorXd::Ones(c_coeffs.rows());

    for (int i = 1; i < c_coeffs.rows(); ++i) {
        x_pow[i] = x_pow[i - 1] * x;
        y_pow[i] = y_pow[i - 1] * y;
    }

    for (int n = 0; n < c_coeffs.rows(); ++n) {
        for (int m = 0; m < c_coeffs.cols(); ++m) {
            auto pair = calc_zernike_polynom(x_pow, y_pow, n, m);
            res += c_coeffs(n, m) * pair.first + s_coeffs(n, m) * pair.second;
        }
    }
    return res;
}

}