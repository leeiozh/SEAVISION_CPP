//
// Created by leeiozh on 7/30/23.
//

#ifndef SEAVISION_RADONIMAGE_HPP
#define SEAVISION_RADONIMAGE_HPP

#include <Eigen/Dense>
#include "SeaVision/Consts.hpp"
#include "SeaVision/MainProcess/Math.hpp"

namespace SeaVision {

class RadonImage {

private:
    Eigen::VectorXd mean_rose;
    int win = 10;

public:
    RadonImage(const int win);

    void update(const Eigen::MatrixXd &back, const int index);

    Eigen::MatrixXd calc_radon(const Eigen::MatrixXd &back);

    Eigen::VectorXd get_directions(const int num);

    Eigen::VectorXd get_rose();

};

} // namespace

#endif //SEAVISION_RADONIMAGE_HPP
