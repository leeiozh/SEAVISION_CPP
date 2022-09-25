//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_CONSTS_HPP
#define POLYGON_CONSTS_HPP

#include <eigen3/Eigen/Dense>

namespace Polygon {

const int COLOR_NUMBER = 1; // число моделируемых цветов

constexpr double rad = M_PI / 180; // один радиан
constexpr double deg = 180 / M_PI; // один градус

// константа в формуле перевода интенсивности в пикселы
constexpr double PLUNK_CONSTANT_LIGHT_SPEED = 6.62607015 * 2.99792458;

const double K_ATMOSPHERE = 0.2; // коэффициент поглощения в атмосферы

const double SUN_TSI = 1361; // солнечная постоянная
const double STAR_BRIGHT = 2.6659876e-9; // яркость опорной звезды (Вега) // TODO: ЗДЕСЬ МАНТИССА - ОКОЛОПОДГОН, ПЕРЕСЧИТАТЬ
const double STAR_MAG = 0.03; // видимая звездная величина опорной звезды (Вега)

}

#endif //POLYGON_CONSTS_HPP
