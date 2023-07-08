//
// Created by leeiozh on 01.09.22.
//

#include <fstream>
#include "SeaVision/Interpolant/Mesh.hpp"
#include "gtest/gtest.h"

const double TOLERANCE = 1e-13;

namespace SeaVision {

TEST(TEST_AREA, AREA_SIMPLE) {

    ReadParameters params{0, 400, 400, 400};

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(375, 375, 0, 0, 0);

    Eigen::MatrixXi back = Eigen::MatrixXi::Zero(600, 600);

    for (int i = 0; i < 600; ++i) {
        for (int j = 0; j < 20; ++j) {
            for (int k = 0; k < 5; ++k) {
                back(20 * j + k, i) = 1.;
                back(i, 20 * j + k) = 2.;
            }
        }
    }

    Eigen::MatrixXd res = mesh.calc_back(area, back);

    ASSERT_FALSE(std::abs(res(0, 0) - 1.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(10, 10) - 0.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(100, 0) - 2.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(24, 0) + res(25, 0) - 1.) > 1e-2);

    /*
    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_matrix.csv");
    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out << res(i, j) << ",";
        }
        out << std::endl;
    }*/
}
}
