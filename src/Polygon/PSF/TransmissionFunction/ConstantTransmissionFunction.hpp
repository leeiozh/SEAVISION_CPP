//
// Created by leeiozh on 31.08.22.
//

#ifndef POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
#define POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP

#include "BaseTransmissionFunction.hpp"

namespace Polygon {

class ConstantTransmissionFunction : public BaseTransmissionFunction {

public:

    inline explicit ConstantTransmissionFunction(const double &trans_coeff) : BaseTransmissionFunction(trans_coeff) {}

    [[nodiscard]] inline double calc_trans_func(const double &x, const double &y) const override {
        return std::sqrt(trans_coeff);
    }

};

}

#endif //POLYGON_CONSTANTTRANSMISSIONFUNCTION_HPP
