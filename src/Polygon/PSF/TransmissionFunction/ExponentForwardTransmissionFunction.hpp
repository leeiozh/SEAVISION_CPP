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
    double calc_trans_func(const double &x, const double &y) const override {
        return 1 - std::exp(-trans_coeff * (x * x + y * y));
    }

};

}

#endif //POLYGON_EXPONENTFORWARDTRANSMISSIONFUNCTION_HPP
