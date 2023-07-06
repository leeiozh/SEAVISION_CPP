//
// Created by leeiozh on 7/5/23.
//

#include "Math.hpp"

namespace SeaVision {

int argmax(const Eigen::VectorXd &vec) {
    int res = 0;
    double max = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        if (max > vec[i]) {
            res = i;
            max = vec[i];
        }
    }
    return res;
}

std::pair<int, int> argmax(const Eigen::MatrixXd &mat) {
    std::pair<int, int> res = std::make_pair(0, 0);
    double max = mat(0, 0);
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            if (max > mat(i, j)) {
                max = mat(i, j);
                res = std::make_pair(i, j);
            }
        }
    }
    return res;
}

int argmax(const Eigen::MatrixXd &mat, int axis) {

    if (axis == 0) {
        Eigen::VectorXd vec;
        for (int i = 0; i < mat.cols(); ++i) {
            vec[i] = mat.col(i).maxCoeff();
        }
        return argmax(vec);
    }
    if (axis == 1) {
        Eigen::VectorXd vec;
        for (int i = 0; i < mat.rows(); ++i) {
            vec[i] = mat.row(i).maxCoeff();
        }
        return argmax(vec);
    } else {
        std::stringstream buff;
        buff << "Support 0 or 1 axis!" << std::endl;
        throw SeaVisionException(buff.str().c_str());
    }
}

Eigen::VectorXd trapezoid(const Eigen::MatrixXd &matrix) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(matrix.rows());
    for (int i = 0; i < matrix.rows(); ++i) {
        res[i] = 0.;
        for (int j = 1; j < matrix.cols(); ++j) {
            res[i] += 0.5 * (matrix(i, j) + matrix(i, j - 1));
        }
    }
    return res;
}

double trapezoid(const Eigen::VectorXd &vector) {
    double res = 0.;
    for (int j = 1; j < vector.size(); ++j) {
        res += 0.5 * (vector(j) + vector(j - 1));
    }
    return res;
}

Eigen::VectorXd operator*(const Eigen::VectorXd &vec1, const Eigen::VectorXd &vec2) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(vec1.size());
    for (int i = 0; i < vec1.size(); ++i) {
        res[i] = vec1[i] * vec2[i];
    }
    return res;
}

} // namespace