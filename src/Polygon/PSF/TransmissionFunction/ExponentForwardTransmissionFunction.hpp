//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP
#define POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"
#include <cmath>

namespace Polygon {

class ExponentForwardTransmissionFunction : public BaseTransmissionFunction {
    /**
     * класс функции пропускания оптической системы с экспоненциальным распределением с убыванием к центру
     */

public:

    /**
     * конструктор по коэффициенту пропускания оптической системы
     * @param trans_coeff коэффициент пропускания
     */
    inline explicit ExponentForwardTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(
            trans_coeff) {}

    /**
     * подсчет функции пропускания в точке (x, y)
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return значение функции пропускания
     */
    [[nodiscard]] inline double calc_trans_func(const double &x, const double &y) const override {
        return std::sqrt(std::exp((1 - trans_coeff) * (x * x + y * y)) * trans_coeff);
    }

};

}

#endif //POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP
