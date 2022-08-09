//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_SCOPE_HPP
#define POLYGON_SCOPE_HPP

#include <utility>

#include "../Utility/Structs.hpp"

namespace Polygon {

class Scope {
    Eigen::Vector3d lambdas = {700., 546.1, 435.8};
    OpticParams optic_params_;
    MatrixParams matrix_params_;
    Eigen::Matrix<double, 1, 1> psf_matrix;

public:
    Scope(Eigen::Vector3d lambdas, struct OpticParams optic_params,
          const struct MatrixParams &matrix_params) : lambdas(std::move(lambdas)),
                                                      optic_params_(std::move(optic_params)),
                                                      matrix_params_(matrix_params) {
        psf_matrix.resize(matrix_params_.pix_Nx, matrix_params_.pix_Ny);
    }
};

}

#endif //POLYGON_SCOPE_HPP
