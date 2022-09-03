//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_MATRIX_HPP
#define POLYGON_MATRIX_HPP

#include <utility>

#include "../Utility/Structs.hpp"
#include "../Utility/Consts.hpp"
#include "../Utility/Functions.hpp"
#include "../Picture/Converters.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class Matrix {
protected:
    MatrixParams params;
    std::array<double, COLOR_NUMBER> light_length;
    std::array<double, COLOR_NUMBER> quant_effiency;
    std::array<Eigen::MatrixXd, COLOR_NUMBER> pictures;
public:

    Matrix(const MatrixParams &matrixParams,
           std::array<Eigen::MatrixXd, COLOR_NUMBER>  pictures,
           const std::array<double, COLOR_NUMBER> &light_length,
           const std::array<double, COLOR_NUMBER> &quant_effiency) : params(matrixParams), pictures(std::move(pictures)),
                                                                     light_length(light_length),
                                                                     quant_effiency(quant_effiency) {}

    void to_electrons(const double &time_expose) {
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    pictures[pic](i, j) = intens_to_electrons(pictures[pic](i, j), time_expose,
                                                                       light_length[pic], quant_effiency[pic], params);
                }
            }
        }
    }

    void to_electrons_with_noise(const double &time_expose) {
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    pictures[pic](i, j) = intens_to_electrons_with_noise(pictures[pic](i, j), time_expose,
                                                                       light_length[pic], quant_effiency[pic], params);
                }
            }
        }
    }

    double operator()(int i, int j, int k){
        return pictures[i](j, k);
    }


};
}

#endif //POLYGON_MATRIX_HPP
