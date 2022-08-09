//
// Created by leeiozh on 08.08.22.
//
#include "gtest/gtest.h"
#include "../src/Polygon/PSF/PSF.hpp"

namespace Polygon {

TEST(TEST_PSF, PSF_SIMPLE) {

    const int SIZE_X = 10;
    const int SIZE_Y = 10;

    Eigen::MatrixXd c_coeffs(3, 2);
    c_coeffs << 1, 1, 0, 0, 2, 0;
    Eigen::MatrixXd s_coeffs(3, 2);
    s_coeffs << 0, 2, 0, 1, 1, 0;

    auto aber_func_ptr = std::make_shared<AberrationFunction>(c_coeffs, s_coeffs);
    auto trans_func_ptr = std::make_shared<ExponentForwardTransmissionFunction>(0.99);
    auto delta_func_ptr = std::make_shared<CircularDeltaFunction>();

    PSF<SIZE_X, SIZE_Y> psf(aber_func_ptr, delta_func_ptr, trans_func_ptr);

    std::cout << psf.calc_psf(0, 0);

    for (int i = 0; i < SIZE_X; ++i) {
        double x = static_cast<double>(i) / SIZE_X;
        for (int j = 0; j < SIZE_Y; ++j) {
            double y = static_cast<double>(j) / SIZE_Y;
            std::cout << i << " " << j << " " << psf.calc_psf(x, y) << std::endl;
        }
    }
}

}