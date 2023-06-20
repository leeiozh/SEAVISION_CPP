//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_CELL_HPP
#define SEAVISION_CELL_HPP

#include <eigen3/Eigen/Dense>

namespace SeaVision {

struct Cell {
    /**
     * structure for cell consists pair of indexes and pair of weights for bilinear interpolation
     */
    Eigen::Vector2i indexes = Eigen::Vector2i::Zero(); // index x and y in polar of current cell in cartesian
    Eigen::Vector2d weights = Eigen::Vector2d::Zero(); // weights of point rho_i and theta_i
};

}

#endif //SEAVISION_CELL_HPP
