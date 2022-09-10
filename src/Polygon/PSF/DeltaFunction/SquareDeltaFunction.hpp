//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_SQUAREDELTAFUNCTION_HPP
#define POLYGON_SQUAREDELTAFUNCTION_HPP

#include "BaseDeltaFunction.hpp"
#include <cmath>

namespace Polygon {

class SquareDeltaFunction : public BaseDeltaFunction {
    /**
     * функция диафрагмирования в форме прямоугольника
     */

public:
    /**
     * вычисление функции диафрагмирования
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return открыта ли эта точка
     */
    [[nodiscard]] inline bool calc_delta_func(const double &x, const double &y) const override {
        return ((std::abs(x) <= 1) && (std::abs(y) <= 1));
    };
};

} // namespace Polygon

#endif //POLYGON_SQUAREDELTAFUNCTION_HPP
