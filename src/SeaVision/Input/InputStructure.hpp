//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTSTRUCTURE_HPP
#define SEAVISION_INPUTSTRUCTURE_HPP

#include <Eigen/Dense>

namespace SeaVision{

    struct InputStructure {

        double cog, sog, giro;
        double lat, lon;
        double step;
        int size = 4096;
        Eigen::MatrixXi bcksctr = Eigen::MatrixXi::Zero(size, size);

    };
}

#endif //SEAVISION_INPUTSTRUCTURE_HPP
