//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_CONSTS_HPP
#define POLYGON_CONSTS_HPP

#include <eigen3/Eigen/Dense>

namespace Polygon {

const int COLOR_NUMBER = 3;

constexpr double rad = M_PI / 180;
constexpr double deg = 180 / M_PI;

const double LIGHT_SPEED = 299792458;
const double PLUNK_CONSTANT_34 = 6.62607015;

const double K_ATMOSPHERE = 0.2;

double STAR_BRIGHT = 1.;
double STAR_MAG = 0.;

}

#endif //POLYGON_CONSTS_HPP
