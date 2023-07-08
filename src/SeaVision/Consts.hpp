//
// Created by leeiozh on 7/6/23.
//

#ifndef SEAVISION_CONSTS_HPP
#define SEAVISION_CONSTS_HPP

#include <cmath>

namespace SeaVision{

constexpr double TURN_PERIOD = 2.5;
constexpr double CUT_NUM = 32;
constexpr double K_MAX = 2. * M_PI / 720. * 32.;
constexpr double G_COEFF = 9.81;
constexpr double DELTA_FREQ = 15;
constexpr int FOUR_NUM = 256;
constexpr double BETA = -1.2;
constexpr int ROSE_SIZE = 180;
constexpr double A_COEFF = 0.;
constexpr double B_COEFF = 4.;

}

#endif //SEAVISION_CONSTS_HPP
