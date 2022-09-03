//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP
#define POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"
#include <cmath>

namespace Polygon {

class ExponentForwardTransmissionFunction : public BaseTransmissionFunction {

public:

    explicit ExponentForwardTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(trans_coeff) {}

    [[nodiscard]] double calc_trans_func(const double &x, const double &y) const override {
        return std::exp((1-trans_coeff) * (x * x + y * y)) * trans_coeff;
    }

};

}

#endif //POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP
