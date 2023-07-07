//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_OUTPUTSTRUCTURE_HPP
#define SEAVISION_OUTPUTSTRUCTURE_HPP

#include <eigen3/Eigen/Core>
#include "SeaVision/Consts.hpp"

namespace SeaVision {

struct OutputStructure {

    Eigen::Vector3d swh = Eigen::Vector3d::Zero();
    Eigen::Vector3d per = Eigen::Vector3d::Zero();
    Eigen::Vector3d dir = Eigen::Vector3d::Zero();
    Eigen::Vector3d len = Eigen::Vector3d::Zero();

    Eigen::VectorXd rose = Eigen::VectorXd::Zero(ROSE_SIZE);

};
}

#endif //SEAVISION_OUTPUTSTRUCTURE_HPP
