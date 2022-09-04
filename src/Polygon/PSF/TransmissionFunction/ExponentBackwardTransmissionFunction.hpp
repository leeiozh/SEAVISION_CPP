//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP
#define POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"
#include <cmath>

namespace Polygon {

class ExponentBackwardTransmissionFunction : public BaseTransmissionFunction {

public:

    inline explicit ExponentBackwardTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(trans_coeff) {}

    [[nodiscard]] inline double calc_trans_func(const double &x, const double &y) const override {
        return std::sqrt(std::exp((trans_coeff - 1) * (x * x + y * y)));
    }

};

}

#endif //POLYGON_EXPONENTBACKWARDTRANSMISSIONFUNCTION_HPP
