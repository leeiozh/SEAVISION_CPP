//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
#define POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"

namespace Polygon {

class ConstantTransmissionFunction : public BaseTransmissionFunction {
    /**
     * класс функции пропускания оптической системы с равномерным распределением
     */

public:

    /**
     * конструктор по коэффициенту пропускания оптической системы
     * @param trans_coeff коэффициент пропускания
     */
    inline explicit ConstantTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(trans_coeff) {}

    /**
     * подсчет функции пропускания в точке (x, y)
     * @param x координата
     * @param y координата
     * @return значение функции пропускания
     */
    [[nodiscard]] inline double calc_trans_func(const double &x, const double &y) const override {
        return std::sqrt(trans_coeff);
    }

};

}

#endif //POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
