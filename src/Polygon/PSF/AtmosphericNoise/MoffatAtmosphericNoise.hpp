//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_MOFFATATMOSPHERICNOISE_HPP
#define POLYGON_MOFFATATMOSPHERICNOISE_HPP

#include "BaseAtmosphericNoise.hpp"
#include "cmath"

namespace Polygon {

class MoffatAtmosphericNoise : public BaseAtmosphericNoise {
    /**
     * класс атмосферного шума в модели Моффата
     */
protected:
    double beta; // коэффициент модели Моффата
    double alpha; // коэффициент модели Моффата
public:
    /**
     * конструктор класса атмосферного шума в модели Моффата
     * @param hwrw относительный радиус размытия точки
     * @param beta коэффициент модели Моффата
     */
    inline MoffatAtmosphericNoise(const double &hwrw, const double &beta) : BaseAtmosphericNoise(hwrw), beta(beta) {
        alpha = hwrw / (2 * std::sqrt(std::pow(2, 1 / beta)) - 1);
    };

    /**
     * вычисление функции атмосферного шума в виде дополнительной фрт
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return значение функции в данной точке
     */
    [[nodiscard]] double calc_atm_noise(const double &x, const double &y) const override;
};

}

#endif //POLYGON_MOFFATATMOSPHERICNOISE_HPP
