//
// Created by leeiozh on 03.09.22.
//

#include "DiegoAtmosphericNoise.hpp"

namespace Polygon {

double DiegoAtmosphericNoise::calc_atm_noise(const double &x, const double &y) const {
    double r = std::sqrt(x * x + y * y);
    return 1 / (1 + std::pow((r / hwrw), p_coeff * (1 + r / pr_coeff)));
}

}