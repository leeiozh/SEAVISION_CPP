//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_BASEDELTAFUNCTION_HPP
#define POLYGON_BASEDELTAFUNCTION_HPP

namespace Polygon {

class BaseDeltaFunction {
    /**
     * базовый класс функции диафрагмирования
     */
public:
    /**
     * вычисление функции диафрагмирования
     * @param x координата х в предметной плоскости
     * @param y координата у в предметной плоскости
     * @return открыта ли эта точка
     */
    [[nodiscard]] inline virtual bool calc_delta_func(const double &x, const double &y) const = 0;
};

} // namespace Polygon

#endif //POLYGON_BASEDELTAFUNCTION_HPP
