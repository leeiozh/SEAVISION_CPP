//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_SQUAREDELTAFUNCTION_HPP
#define POLYGON_SQUAREDELTAFUNCTION_HPP

#include "BaseDeltaFunction.hpp"
#include <cmath>

namespace Polygon {

class SquareDeltaFunction : public BaseDeltaFunction {

public:
    [[nodiscard]] inline bool calc_delta_func(const double &x, const double &y) const override {
        return ((std::abs(x) <= 1) && (std::abs(y) <= 1));
    };
};

} // namespace Polygon

#endif //POLYGON_SQUAREDELTAFUNCTION_HPP
