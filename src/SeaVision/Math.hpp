//
// Created by leeiozh on 7/5/23.
//

#ifndef SEAVISION_MATH_HPP
#define SEAVISION_MATH_HPP

#include <Eigen/Dense>
#include "SeaVision/SeaVisionBaseException.hpp"
#include "SeaVision/Consts.hpp"

namespace SeaVision {

int argumax(const Eigen::VectorXi &vec);

int argumax(const Eigen::VectorXd &vec);

int count_zeros_mean(const Eigen::VectorXd &vec);

int get_median(const Eigen::VectorXi &dir_ind, bool change_mean);

std::pair<int, int> argumax(const Eigen::MatrixXd &mat);

Eigen::VectorXi argumax(const Eigen::VectorXd &vec, int num, int window);

Eigen::VectorXi argumax(const Eigen::MatrixXd &mat, int axis);

Eigen::VectorXd gradient_fir(const Eigen::VectorXi &vec);

Eigen::VectorXd gradient_sec(const Eigen::VectorXi &vec);

Eigen::VectorXd abs(const Eigen::VectorXd &vec);

double trapezoid(const Eigen::VectorXd &matrix, int start = 0, int end = -1);

Eigen::VectorXd trapezoid(const Eigen::MatrixXd &matrix, int start = 0, int end = -1);

Eigen::VectorXd operator*(const Eigen::VectorXd &vec1, const Eigen::VectorXd &vec2); // termwise multiplication


} // namespace

#endif //SEAVISION_MATH_HPP
