//
// Created by leeiozh on 6/16/23.
//

#ifndef SEAVISION_OUTPUTSTRUCTURE_HPP
#define SEAVISION_OUTPUTSTRUCTURE_HPP

#include <Eigen/Dense>

namespace SeaVision {

struct InputStructure {

    std::array<double, 3> swh;
    std::array<double, 3> per;
    std::array<double, 3> dir;
    std::array<double, 3> len;

    Eigen::VectorXd rose;

};
}

#endif //SEAVISION_OUTPUTSTRUCTURE_HPP
