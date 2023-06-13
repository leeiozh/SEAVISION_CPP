//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_CELL_HPP
#define SEAVISION_CELL_HPP

#include <eigen3/Eigen/Dense>

namespace SeaVision {

struct Cell {
    Eigen::Vector2i indexes = Eigen::Vector2i::Zero();
    Eigen::Vector2d weights = Eigen::Vector2d::Zero();
};

}

#endif //SEAVISION_CELL_HPP
