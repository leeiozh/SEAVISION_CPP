//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
#define POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"

namespace Polygon {

class ConstantTransmissionFunction : public BaseTransmissionFunction {

public:

    explicit ConstantTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(trans_coeff) {}

    [[nodiscard]] double calc_trans_func(const double &x, const double &y) const override {
        return trans_coeff;
    }

};

}

#endif //POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
