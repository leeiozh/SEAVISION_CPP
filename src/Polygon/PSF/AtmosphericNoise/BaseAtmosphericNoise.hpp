//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_BASEATMOSPHERICNOISE_HPP
#define POLYGON_BASEATMOSPHERICNOISE_HPP

namespace Polygon {

class BaseAtmosphericNoise {
protected:
    double hwrw = 0;
public:
    explicit BaseAtmosphericNoise(const double &hwrw) : hwrw(hwrw) {};

    [[nodiscard]] virtual double calc_atm_noise(const double &x, const double &y) const = 0;
};

}

#endif //POLYGON_BASEATMOSPHERICNOISE_HPP
