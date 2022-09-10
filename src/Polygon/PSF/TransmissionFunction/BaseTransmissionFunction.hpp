//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_BASETRANSMISSIONFUNCTION_HPP
#define POLYGON_BASETRANSMISSIONFUNCTION_HPP

namespace Polygon {

class BaseTransmissionFunction {
    /**
     * базовый класс функции пропускания оптической системы
     */
protected:
    double trans_coeff = 1; // коэффициент пропускания оптической системы

public:

    // базовый конструктор
    inline BaseTransmissionFunction() = default;

    /**
     * конструктор по коэффициенту пропускания
     * @param trans_coeff коэффициент пропускания
     */
    inline explicit BaseTransmissionFunction(const double &trans_coeff) : trans_coeff(trans_coeff) {}

    /**
     * подсчет функции пропускания в точке (x, y)
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return функция пропускания
     */
    [[nodiscard]] inline virtual double calc_trans_func(const double &x, const double &y) const = 0;
};

} // namespace Polygon

#endif //POLYGON_BASETRANSMISSIONFUNCTION_HPP
