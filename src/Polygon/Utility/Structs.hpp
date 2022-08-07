//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_STATES_HPP
#define POLYGON_STATES_HPP

#include <string>
#include <eigen3/Eigen/Dense>

namespace Polygon {

struct sat_state {
    int number;
    double radius;
    Eigen::Vector3d position;
    Eigen::Vector3d velocity;
};

struct star {
    int area_number;
    int inarea_number;
    int instar_component;
    double ascension;
    double declination;
    double magnitude;
};

struct exter_params {
    double hwhm = 0;
    double p_diego = 0;
    double pr_diego = 0;
    double b_moffat = 3;
};

struct optic_params {
    double inp_diam;
    double out_diam;
    double trans_coeff;
    int iso_number = 1;
    std::vector<double> c_coeffs;
    std::vector<double> s_coeffs;
    exter_params exter_params;
};

struct noise_params {
    int warm;
    int count;
};

struct matrix_params {
    double pix_x;
    double pix_y;
    double pix_Nx;
    double pix_Ny;
    double matr_x;
    double matr_y;
    noise_params noise_params;
    double pix_capacity;
    double quant_efficiency;
};


} // namespace Polygon

#endif //POLYGON_STATES_HPP
