//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_BASETRANSMISSIONFUNCTION_HPP
#define POLYGON_BASETRANSMISSIONFUNCTION_HPP

namespace Polygon {

class BaseTransmissionFunction {
protected:
    double trans_coeff = 1;
public:
    BaseTransmissionFunction() = default;

    explicit BaseTransmissionFunction(const double &trans_coeff) : trans_coeff(trans_coeff) {}

    [[nodiscard]] virtual double calc_trans_func(const double &x, const double &y) const = 0;
};

} // namespace Polygon

#endif //POLYGON_BASETRANSMISSIONFUNCTION_HPP
