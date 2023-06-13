//
// Created by leeiozh on 01.09.22.
//

#include <fstream>
#include "SeaVision/Interpolant/Mesh.hpp"
#include "gtest/gtest.h"

namespace SeaVision {

TEST(TEST_AREA, AREA_SIMPLE) {

    Mesh mesh = Mesh(4000, 4000, 1.875);
    Area area = Area(720, 720, -90, 90, 720);

    Eigen::MatrixXi back = Eigen::MatrixXi::Zero(4000, 4000);

    for (int i = 0; i < 4000; ++i) {
        for (int j = 0; j < 200; ++j) {
            for (int k = 0; k < 10; ++k){
                back(i, 20 * j + k) = 1.;
            }
            //back(10 * j, i) = 2.;
        }
    }

    Eigen::MatrixXd res = mesh.calc_back(area, back);

    std::ofstream out("/home/leeiozh/ocean/seavision/test1.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out << res(i, j) << ",";
        }
        out << std::endl;
    }
}
}
