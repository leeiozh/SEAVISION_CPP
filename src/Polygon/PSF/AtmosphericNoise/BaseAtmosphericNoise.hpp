//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_BASEATMOSPHERICNOISE_HPP
#define POLYGON_BASEATMOSPHERICNOISE_HPP

namespace Polygon {

class BaseAtmosphericNoise {
    /**
     * базовый класс функции атмосферного шума в виде дополнительной функции размытия точки
     */
protected:
    double hwrw = 0; // относительный радиус функции размытия точки (есть в любом типе подобного шума)
public:
    // базовый конструктор
    inline BaseAtmosphericNoise() = default;

    /**
     * конструктор по функции размытия точки
     * @param hwrw радиус функции размытия точки
     */
    inline explicit BaseAtmosphericNoise(const double &hwrw) : hwrw(hwrw) {};

    /**
     * вычисление дополнительной функции размытия точки
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return значение функции в данной точке
     */
    [[nodiscard]] inline virtual double calc_atm_noise(const double &x, const double &y) const = 0;
};

}

#endif //POLYGON_BASEATMOSPHERICNOISE_HPP
