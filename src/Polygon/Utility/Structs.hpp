//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_STATES_HPP
#define POLYGON_STATES_HPP

#include <string>
#include <eigen3/Eigen/Dense>

namespace Polygon {

struct ScopeState {
    Eigen::Vector3d position;
    Eigen::Vector3d direction;
};

struct SatState {
    int number;
    double radius;
    Eigen::Vector3d position3d;
    Eigen::Vector2d position2d = Eigen::Vector2d::Zero();
    double bright = 0.;
};

struct Star {
    int area_number;
    int inarea_number;
    int instar_component;
    double ascension;
    double declination;
    double magnitude;
    double bright;
    Eigen::Vector2d position2d;
};

struct ExterParams {
    double hwhm = 0;
    double p_diego = 0;
    double pr_diego = 0;
    double b_moffat = 3;
};

struct OpticParams {
    double inp_diam;
    double out_diam;
    double trans_coeff;
    int iso_number = 1;
    Eigen::MatrixXd c_coeffs;
    Eigen::MatrixXd s_coeffs;
    ExterParams exter_params;
};

struct NoiseParams {
    int warm;
    int count;
};

struct MatrixParams {
    double pix_x;
    double pix_y;
    double matr_x;
    double matr_y;
    NoiseParams noise_params;
    double pix_capacity;
};


} // namespace Polygon

#endif //POLYGON_STATES_HPP
