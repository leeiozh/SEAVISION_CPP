//
// Created by leeiozh on 01.09.22.
//

#include <fstream>
#include "gtest/gtest.h"
#include "../src/Polygon/Matrix/Matrix.hpp"

namespace Polygon {

TEST(TEST_MATRIX, MATRIX_SIMPLE) {

    const int SIZE_X = 200;
    const int SIZE_Y = 200;

    Polygon::NoiseParams noiseParams{5, 5};
    Polygon::MatrixParams matrixParams{0.1, 0.1, SIZE_X, SIZE_Y, 2, 2, noiseParams, 330};

    std::array<Eigen::MatrixXd, COLOR_NUMBER> pics;
    pics[0] = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);

    for (int i = 0; i < SIZE_X; ++i){
        pics[0](i, i) = 1e-6;
    }

    pics[1] = 1.5e-6 * Eigen::MatrixXd::Ones(SIZE_X, SIZE_Y);
    pics[2] = 2e-6 * Eigen::MatrixXd::Ones(SIZE_X, SIZE_Y);

    std::array<double, 3> lengths = {700., 546.1, 435.8};
    std::array<double, 3> quant_eff = {0.9, 0.99, 0.8};

    Matrix<SIZE_X, SIZE_Y> matrix(matrixParams, pics, lengths, quant_eff);

    matrix.to_electrons_with_noise(1e-6);

    std::ofstream out("/home/leeiozh/mmcp/calculation_results/Matrix.csv");

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            out << matrix(0, i, j) << ",";
        }
        out << std::endl;
    }
}

}