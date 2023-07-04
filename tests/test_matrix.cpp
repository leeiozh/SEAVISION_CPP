//
// Created by leeiozh on 01.09.22.
//

#include <fstream>
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/Input/InputStructure.hpp"
#include "gtest/gtest.h"

namespace SeaVision {

TEST(TEST_AREA, AREA_SIMPLE) {

    ReadParameters params{0, 400, 400, 400};

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(375, 375, 0, 0, 0);

    Eigen::MatrixXi back = Eigen::MatrixXi::Zero(600, 600);

    for (int i = 0; i < 600; ++i) {
        for (int j = 0; j < 20; ++j) {
            for (int k = 0; k < 10; ++k){
                back(i, 20 * j + k) = 1.;
            }
            // back(10 * j, i) = 2.;
        }
    }

    Eigen::MatrixXd res = mesh.calc_back(area, back);

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test2.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out << res(i, j) << ",";
        }
        out << std::endl;
    }
}
}
