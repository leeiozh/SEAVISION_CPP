//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_CONSTS_HPP
#define POLYGON_CONSTS_HPP

#include <eigen3/Eigen/Dense>

namespace Polygon {

const int COLOR_NUMBER = 1;

constexpr double rad = M_PI / 180;
constexpr double deg = 180 / M_PI;

constexpr double PLUNK_CONSTANT_LIGHT_SPEED = 6.62607015 * 2.99792458;

const double K_ATMOSPHERE = 0.2;

const double SUN_TSI = 1361;
const double STAR_BRIGHT = 0.032293;
const double STAR_MAG = 0.03;

}

#endif //POLYGON_CONSTS_HPP
