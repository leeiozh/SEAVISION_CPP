//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_MOFFATATMOSPHERICNOISE_HPP
#define POLYGON_MOFFATATMOSPHERICNOISE_HPP

#include "BaseAtmosphericNoise.hpp"
#include "cmath"

namespace Polygon {

class MoffatAtmosphericNoise : public BaseAtmosphericNoise {
protected:
    double beta;
public:
    MoffatAtmosphericNoise(const double &hwrw, const double &beta) : BaseAtmosphericNoise(hwrw), beta(beta) {};

    [[nodiscard]] double calc_atm_noise(const double &x, const double &y) const override {
        double r2 = x * x + y * y;
        return 2 * (beta - 1) / hwrw * std::pow(1 + r2 / hwrw / hwrw, beta);
    }
};

}

#endif //POLYGON_MOFFATATMOSPHERICNOISE_HPP
