//
// Created by leeiozh on 7/5/23.
//

#ifndef SEAVISION_MATH_HPP
#define SEAVISION_MATH_HPP

#include <Eigen/Dense>
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

int argmax(const Eigen::VectorXd &vec);
std::pair<int, int> argmax(const Eigen::MatrixXd &mat);
int argmax(const Eigen::MatrixXd &mat, int axis);

double trapezoid(const Eigen::VectorXd &matrix);
Eigen::VectorXd trapezoid(const Eigen::MatrixXd &matrix);

Eigen::VectorXd operator *(const Eigen::VectorXd& vec1, const Eigen::VectorXd& vec2);


} // namespace

#endif //SEAVISION_MATH_HPP
