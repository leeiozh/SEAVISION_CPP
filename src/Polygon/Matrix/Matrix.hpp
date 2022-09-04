//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_MATRIX_HPP
#define POLYGON_MATRIX_HPP

#include "../Utility/Structs.hpp"
#include "../Utility/Consts.hpp"
#include "../Utility/Functions.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class Matrix {
protected:
    MatrixParams params;
    std::array<double, COLOR_NUMBER> light_length;
    std::array<double, COLOR_NUMBER> quant_effiency;
    std::array<Eigen::MatrixXd, COLOR_NUMBER> pictures;
public:

    inline Matrix(const MatrixParams &matrixParams,
           const std::array<double, COLOR_NUMBER> &light_length,
           const std::array<double, COLOR_NUMBER> &quant_effiency) : params(matrixParams),
                                                                     light_length(light_length),
                                                                     quant_effiency(quant_effiency) {}

    inline std::array<Eigen::MatrixXi, COLOR_NUMBER> to_electrons(const double &time_expose) {
        std::array<Eigen::MatrixXi, COLOR_NUMBER> res;
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            res[pic] = Eigen::MatrixXi::Zero(SIZE_X, SIZE_Y);
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    double elec = pictures[pic](i, j) * time_expose * params.pix_x * params.pix_y * light_length[pic] /
                            PLUNK_CONSTANT_LIGHT_SPEED * 1e17;
                    elec *= quant_effiency[pic];
                    elec = std::min(elec, params.pix_capacity);
                    elec = std::max(elec, 0.);
                    res[pic](i, j) = static_cast<int>(elec);
                }
            }
        }
        return res;
    }

    inline std::array<Eigen::MatrixXi, COLOR_NUMBER> to_electrons_with_noise(const double &time_expose) {

        std::array<Eigen::MatrixXi, COLOR_NUMBER> res;
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            res[pic] = Eigen::MatrixXi::Zero(SIZE_X, SIZE_Y);
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    double elec = pictures[pic](i, j) * time_expose * params.pix_x * params.pix_y * light_length[pic] /
                                  PLUNK_CONSTANT_LIGHT_SPEED * 1e17;

                    elec *= quant_effiency[pic];
                    elec += normal_random(params.noise_params);
                    elec = std::min(elec, params.pix_capacity);
                    elec = std::max(elec, 0.);

                    res[pic](i, j) = static_cast<int>(elec);
                }
            }
        }

        return res;
    }

    inline double operator()(int i, int j, int k) {
        return pictures[i](j, k);
    }

    inline MatrixParams get_params() {
        return params;
    }

    inline void set_pictures(const std::array<Eigen::MatrixXd, COLOR_NUMBER> &pics) {
        pictures = pics;
    }

};
}

#endif //POLYGON_MATRIX_HPP
