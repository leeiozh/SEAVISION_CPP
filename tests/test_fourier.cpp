//
// Created by leeiozh on 6/13/23.
//

#include <fstream>
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "gtest/gtest.h"

namespace SeaVision {

TEST(TEST_FOURIER, FOURIER) {

    Eigen::MatrixXd data = Eigen::MatrixXd::Zero(512, 512);

    for (int i = 0; i < data.cols(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            data(i, j) = std::cos( M_PI * i / data.cols() * 10) + std::cos( M_PI * i / data.cols() * 20);
        }
    }

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_data1.csv");

    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            out << data(i, j) << ",";
        }
        out << std::endl;
    }

    DispersionCurve dispersionCurve(256, 15, data.cols());

    Eigen::MatrixXcd res = dispersionCurve.calc_fourier_2d(data);

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four1.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j).real() << ",";
        }
        out2 << std::endl;
    }


}
}
