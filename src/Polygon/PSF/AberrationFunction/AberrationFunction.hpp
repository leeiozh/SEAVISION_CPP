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
//
//    Eigen::VectorXi fac_vec;
//
//    Eigen::VectorXi calc_fac(const int &size) const;
//
//    double binom(const int &k, const int &n, const Eigen::VectorXi &fac_vec) const;
//
//    std::pair<double, double>
//    calc_cos_sin(const double &cos_phi, const double &tg_phi, const int &size, const Eigen::VectorXi &fac_vec) const;

    std::pair<double, double>
    calc_zernike_polynom(const Eigen::VectorXd &x_pow, const Eigen::VectorXd &y_pow, const int &n, const int &m) const;

public:

    AberrationFunction(const Eigen::MatrixXd &c_coeffs, const Eigen::MatrixXd &s_coeffs);

    double calc_aberration_func(const double &x, const double &y) const;

};
}

#endif //POLYGON_ABERRATIONFUNCTION_HPP
