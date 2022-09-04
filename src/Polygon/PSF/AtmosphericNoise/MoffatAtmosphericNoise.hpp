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
    double alpha;
public:
    inline MoffatAtmosphericNoise(const double &hwrw, const double &beta) : BaseAtmosphericNoise(hwrw), beta(beta) {
        alpha = hwrw / (2 * std::sqrt(std::pow(2, 1/beta)) - 1);
    };

    [[nodiscard]] double calc_atm_noise(const double &x, const double &y) const override;
};

}

#endif //POLYGON_MOFFATATMOSPHERICNOISE_HPP
