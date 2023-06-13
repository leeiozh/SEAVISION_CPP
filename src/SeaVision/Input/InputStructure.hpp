//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTSTRUCTURE_HPP
#define SEAVISION_INPUTSTRUCTURE_HPP

#include <Eigen/Dense>

namespace SeaVision{

    struct InputStructure {

        double cog, sog, giro;
        double step;
        Eigen::MatrixXd bcksctr;

    };
}

#endif //SEAVISION_INPUTSTRUCTURE_HPP
