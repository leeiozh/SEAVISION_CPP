//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_DIEGOATMOSPHERICNOISE_HPP
#define POLYGON_DIEGOATMOSPHERICNOISE_HPP

#include "BaseAtmosphericNoise.hpp"
#include "cmath"

namespace Polygon {

class DiegoAtmosphericNoise : public BaseAtmosphericNoise {
protected:
    double p_coeff;
    double pr_coeff;
public:
    DiegoAtmosphericNoise(const double &hwrw, const double &p_coeff, const double &pr_coeff) : BaseAtmosphericNoise(
            hwrw), p_coeff(p_coeff), pr_coeff(pr_coeff) {};

    [[nodiscard]] double calc_atm_noise(const double &x, const double &y) const override {
        double r = std::sqrt(x * x + y * y);
        return 1 / (1 + std::pow((r / hwrw), p_coeff * (1 + r / pr_coeff)));
    }
};

}
#endif //POLYGON_DIEGOATMOSPHERICNOISE_HPP
