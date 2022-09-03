//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_SCOPE_HPP
#define POLYGON_SCOPE_HPP

#include <utility>

#include "../Utility/Structs.hpp"
#include "../PSF/PSF.hpp"

namespace Polygon {

template<int SIZE_X, int SIZE_Y>

class Scope {
    Eigen::Vector3d lambdas = {700., 546.1, 435.8};
    OpticParams optic_params_;
    MatrixParams matrix_params_;
    PSF<SIZE_X, SIZE_Y> psf_matrix;
    std::vector<Eigen::Matrix3d> from_own_to_gcrs;

public:
    Scope(Eigen::Vector3d lambdas, struct OpticParams optic_params,
          const struct MatrixParams &matrix_params) : lambdas(std::move(lambdas)),
                                                      optic_params_(std::move(optic_params)),
                                                      matrix_params_(matrix_params) {}
};

}

#endif //POLYGON_SCOPE_HPP
