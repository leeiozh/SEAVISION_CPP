//
// Created by leeiozh on 7/30/23.
//

#include "RadonImage.hpp"

namespace SeaVision {

RadonImage::RadonImage(const int win) : win(win) {
    mean_rose.resize(RADON_DIR_SIZE);
}

Eigen::MatrixXd RadonImage::calc_radon(const Eigen::MatrixXd &back) {
    Eigen::MatrixXd res = Eigen::MatrixXd::Zero(back.rows(), RADON_DIR_SIZE);

    

    return res;
}

void RadonImage::update(const Eigen::MatrixXd &back, const int index) {

    auto back_image = calc_radon(back);

    for (int i = 0; i < RADON_DIR_SIZE; ++i) {
        int max_ind = std::min(index, FOUR_NUM);
        mean_rose[i] *= (1 - 1. / max_ind);
        mean_rose[i] +=
                (back_image.col(i) - back_image.col(i).mean() * Eigen::VectorXd::Ones(back_image.rows())).norm() /
                max_ind;
    }

}

Eigen::VectorXd RadonImage::get_directions(const int num) {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(num);

    Eigen::VectorXd copy = Eigen::VectorXd(mean_rose);

    for (int i = 0; i < num; ++i) {
        int argmax = argumax(copy);
        res[i] = argmax;

        if (i != num - 1) {
            copy.segment(std::min(0, argmax - win), std::min(RADON_DIR_SIZE - argmax, 2 * win)).setZero();

            if (argmax < win) {
                copy.segment(RADON_DIR_SIZE - win + argmax, win - argmax).setZero();
            } else if (argmax + win > RADON_DIR_SIZE) {
                copy.segment(0, argmax + win - RADON_DIR_SIZE).setZero();
            }
        }
    }

    return res;
}

Eigen::VectorXd RadonImage::get_rose() {
    return mean_rose;
}

} // namespace