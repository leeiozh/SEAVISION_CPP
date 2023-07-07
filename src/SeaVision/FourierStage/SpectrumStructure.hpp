//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_SPECTRUMSTRUCTURE_HPP
#define SEAVISION_SPECTRUMSTRUCTURE_HPP

#include <Eigen/Dense>
#include "SeaVision/Consts.hpp"

namespace SeaVision {

struct SpectrumStruct {
    double m0 = 0.;
    double m1 = 0.;
    double peak_period = 0.;
    Eigen::VectorXd freq_spec = Eigen::VectorXd::Zero(FOUR_NUM);

};
} // namespace

#endif //SEAVISION_SPECTRUMSTRUCTURE_HPP
