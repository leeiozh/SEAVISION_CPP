//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_DIEGOATMOSPHERICNOISE_HPP
#define POLYGON_DIEGOATMOSPHERICNOISE_HPP

#include "BaseAtmosphericNoise.hpp"
#include "cmath"

namespace Polygon {

class DiegoAtmosphericNoise : public BaseAtmosphericNoise {
    /**
     * класс атмосферного шума в модели Диего
     */
protected:
    double p_coeff; // коэффициент модели Диего
    double pr_coeff; // коэффицент модели Диего
public:
    /**
     * конструктор атмосферного шума в модели Диего
     * @param hwrw относительный радиус размытия точки
     * @param p_coeff коэффициент модели Диего
     * @param pr_coeff коэффициент модели Диего
     */
    inline DiegoAtmosphericNoise(const double &hwrw, const double &p_coeff, const double &pr_coeff)
            : BaseAtmosphericNoise(
            hwrw), p_coeff(p_coeff), pr_coeff(pr_coeff) {};

    /**
     * вычисление дополнительной функции размытия точки
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return значение функции в данной точке
     */
    [[nodiscard]] double calc_atm_noise(const double &x, const double &y) const override;
};

}
#endif //POLYGON_DIEGOATMOSPHERICNOISE_HPP
