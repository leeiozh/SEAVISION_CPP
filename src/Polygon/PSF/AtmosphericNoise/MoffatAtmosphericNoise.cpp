//
// Created by leeiozh on 03.09.22.
//

#include "MoffatAtmosphericNoise.hpp"

namespace Polygon {

double MoffatAtmosphericNoise::calc_atm_noise(const double &x, const double &y) const {

    double r2 = x * x + y * y; // квадрат расстояния до точечного источника в предметной плоскости

    return std::pow(1 + r2 / alpha / alpha, -beta);
//    return 2 * (beta - 1) / hwrw * std::pow(1 + r2 / hwrw / hwrw, beta);
}

}
