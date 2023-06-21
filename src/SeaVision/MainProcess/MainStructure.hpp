//
// Created by leeiozh on 6/16/23.
//

#ifndef SEAVISION_MAINSTRUCTURE_HPP
#define SEAVISION_MAINSTRUCTURE_HPP

#include <eigen3/Eigen/Dense>

namespace SeaVision {

struct MainStruct {

    std::vector<Eigen::MatrixXi> back;
    std::vector<Eigen::MatrixXd> four;

};


} // namespace

#endif //SEAVISION_MAINSTRUCTURE_HPP
