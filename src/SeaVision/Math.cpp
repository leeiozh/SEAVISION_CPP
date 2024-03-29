//
// Created by leeiozh on 7/5/23.
//

#include "Math.hpp"

namespace SeaVision {

int argumax(const Eigen::VectorXi &vec) {
    int res = 0;
    double max = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        if (max < vec[i]) {
            res = i;
            max = vec[i];
        }
    }
    return res;
}

Eigen::VectorXi argumax(const Eigen::VectorXd &vec, int num, int window) {
    Eigen::VectorXi res = Eigen::VectorXi::Zero(num);
    auto copy = Eigen::VectorXd(vec);
    for (int i = 0; i < num; ++i) {
        res[i] = argumax(copy);

        if (res[i] - window > 0) {
            copy.segment(res[i] - window, window).setZero();
        } else {
            copy.segment(0, res[i]).setZero();
            copy.segment(copy.size() - window + res[i], window - res[i]).setZero();
        }
        if (res[i] + window < vec.size()) {
            copy.segment(res[i], window).setZero();
        } else {
            copy.segment(res[i], vec.size() - res[i]).setZero();
            copy.segment(0, window - vec.size() + res[i]).setZero();
        }
    }
    return res;
}

int argumax(const Eigen::VectorXd &vec) {
    int res = 0;
    double max = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        if (max < vec[i]) {
            res = i;
            max = vec[i];
        }
    }
    return res;
}

int count_zeros_mean(const Eigen::VectorXd &vec) {
    Eigen::VectorXd vec_min_mean = vec - vec.mean() * Eigen::VectorXd::Ones(vec.size());
    int res = 0;
    for (int i = 1; i < vec_min_mean.size(); ++i) {
        if (vec_min_mean[i] * vec_min_mean[i - 1] < 0) res++;
    }
    return res;
}

int get_median(const Eigen::VectorXi &dir_ind, bool change_mean) {

    if (change_mean) {
        auto copy_vec = Eigen::VectorXi(dir_ind);
        std::sort(copy_vec.begin(), copy_vec.end());
        return copy_vec[static_cast<int>(std::round(static_cast<double>(CHANGE_DIR_NUM_SHOTS) / 2))];
    } else {
        auto copy_vec = Eigen::VectorXi(dir_ind.segment(0, MEAN));
        std::sort(copy_vec.begin(), copy_vec.end());
        return copy_vec[static_cast<int>(std::round(static_cast<double>(MEAN) / 2))];
    }
}

std::pair<int, int> argumax(const Eigen::MatrixXd &mat) {
    std::pair<int, int> res = std::make_pair(0, 0);
    double max = mat(0, 0);
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            if (max < mat(i, j)) {
                max = mat(i, j);
                res = std::make_pair(i, j);
            }
        }
    }
    return res;
}

Eigen::VectorXi argumax(const Eigen::MatrixXd &mat, int axis) {

    if (axis == 0) {
        Eigen::VectorXi res = Eigen::VectorXi::Zero(mat.cols());
        for (int i = 0; i < mat.cols(); ++i) {
            res[i] = argumax(Eigen::VectorXd(mat.col(i)));
        }
        return res;
    }
    if (axis == 1) {
        Eigen::VectorXi res = Eigen::VectorXi::Zero(mat.rows());
        for (int i = 0; i < mat.rows(); ++i) {
            res[i] = argumax(Eigen::VectorXd(mat.row(i)));
        }
        return res;
    } else {
        std::stringstream buff;
        buff << "Support 0 or 1 axis!" << std::endl;
        throw SeaVisionException(buff.str().c_str());
    }
}

Eigen::VectorXd gradient_fir(const Eigen::VectorXi &vec) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(vec.size());
    res[0] = 0.;
    for (int i = 1; i < vec.size() - 1; ++i) {
        res[i] = vec[i] - vec[i - 1];
    }
    return res;
}

Eigen::VectorXd gradient_sec(const Eigen::VectorXi &vec) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(vec.size());
    res[0] = vec[1] - vec[0];
    res[vec.size() - 1] = vec[vec.size() - 1] - vec[vec.size() - 2];
    for (int i = 1; i < vec.size() - 1; ++i) {
        res[i] = 0.5 * (vec[i + 1] - vec[i - 1]);
    }
    return res;
}

Eigen::VectorXd abs(const Eigen::VectorXd &vec) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        res[i] = std::abs(vec[i]);
    }
    return res;
}

Eigen::VectorXd trapezoid(const Eigen::MatrixXd &matrix, int start, int end) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(matrix.rows());
    for (int i = 0; i < matrix.rows(); ++i) {
        res[i] = matrix(i, start);
        for (int j = start + 1; j < matrix.cols() + 1 + end; ++j) {
            res[i] += 0.5 * (matrix(i, j) + matrix(i, j - 1));
        }
    }
    return res;
}

double trapezoid(const Eigen::VectorXd &vector, int start, int end) {
    double res = vector(start);
    for (int j = start + 1; j < vector.size() + 1 + end; ++j) {
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