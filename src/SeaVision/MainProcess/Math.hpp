//
// Created by leeiozh on 7/5/23.
//

#ifndef SEAVISION_MATH_HPP
#define SEAVISION_MATH_HPP

#include <Eigen/Dense>
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

int argumax(const Eigen::VectorXi &vec);
int argumax(const Eigen::VectorXd &vec);
std::pair<int, int> argumax(const Eigen::MatrixXd &mat);
Eigen::VectorXi argumax(const Eigen::MatrixXd &mat, int axis);

Eigen::VectorXd gradient(const Eigen::VectorXi &vec);
Eigen::VectorXd abs(const Eigen::VectorXd &vec);

double trapezoid(const Eigen::VectorXd &matrix);
Eigen::VectorXd trapezoid(const Eigen::MatrixXd &matrix);

Eigen::VectorXd operator *(const Eigen::VectorXd& vec1, const Eigen::VectorXd& vec2); // termwise multiplication


} // namespace

#endif //SEAVISION_MATH_HPP
