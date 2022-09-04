//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_DELTAFUNCTION_HPP
#define POLYGON_DELTAFUNCTION_HPP

#include "BaseDeltaFunction.hpp"

namespace Polygon {

class CircularDeltaFunction : public BaseDeltaFunction {

public:
    [[nodiscard]] inline bool calc_delta_func(const double &x, const double &y) const override {
        return ((x * x + y * y) <= 1);
    };
};

} // namespace Polygon

#endif //POLYGON_DELTAFUNCTION_HPP
