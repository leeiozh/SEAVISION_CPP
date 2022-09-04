//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_BASEDELTAFUNCTION_HPP
#define POLYGON_BASEDELTAFUNCTION_HPP

namespace Polygon {

class BaseDeltaFunction {
public:
    [[nodiscard]] inline virtual bool calc_delta_func(const double &x, const double &y) const = 0;
};

} // namespace Polygon

#endif //POLYGON_BASEDELTAFUNCTION_HPP
