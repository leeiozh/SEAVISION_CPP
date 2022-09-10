//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP
#define POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"
#include <cmath>

namespace Polygon {

class ExponentBackwardTransmissionFunction : public BaseTransmissionFunction {
    /**
     * класс функции пропускания оптической системы с экспоненциальным распределением с убыванием к краю
     */

public:

    /**
     * конструктор по коэффициенту пропускания оптической системы
     * @param trans_coeff коэффициент пропускания
     */
    inline explicit ExponentBackwardTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(
            trans_coeff) {}

    /**
     * подсчет функции пропускания в точке (x, y)
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return значение функции пропускания
     */
    [[nodiscard]] inline double calc_trans_func(const double &x, const double &y) const override {
        return std::sqrt(std::exp((trans_coeff - 1) * (x * x + y * y)));
    }

};

}

#endif //POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP
