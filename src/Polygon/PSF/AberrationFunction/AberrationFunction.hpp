//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_ABERRATIONFUNCTION_HPP
#define POLYGON_ABERRATIONFUNCTION_HPP

#include <eigen3/Eigen/Dense>

namespace Polygon {

class AberrationFunction {

protected:
    Eigen::MatrixXd c_coeffs;
    Eigen::MatrixXd s_coeffs;

public:

    [[nodiscard]] static std::pair<double, double>
    calc_zernike_polynom(const Eigen::VectorXd &x_pow, const Eigen::VectorXd &y_pow, const int &n, const int &m);

    AberrationFunction(const Eigen::MatrixXd &c_coeffs, const Eigen::MatrixXd &s_coeffs);

    [[nodiscard]] double calc_aberration_func(const double &x, const double &y) const;

};
}

#endif //POLYGON_ABERRATIONFUNCTION_HPP
