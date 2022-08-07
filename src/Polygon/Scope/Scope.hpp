//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_SCOPE_HPP
#define POLYGON_SCOPE_HPP

#include "../Utility/Structs.hpp"

namespace Polygon {

class Scope {
    Eigen::Vector3d lambdas = {700., 546.1, 435.8};
    optic_params optic_params;
    matrix_params matrix_params;
    Eigen::Matrix<double, 1, 1> psf_matrix;

public:
    Scope(const Eigen::Vector3d &lambdas, const struct optic_params &optic_params,
          const struct matrix_params &matrix_params) : lambdas(lambdas),
                                                       optic_params(optic_params),
                                                       matrix_params(matrix_params) {
        psf_matrix.resize(matrix_params.pix_Nx, matrix_params.pix_Ny);
    }
};

}

#endif //POLYGON_SCOPE_HPP
