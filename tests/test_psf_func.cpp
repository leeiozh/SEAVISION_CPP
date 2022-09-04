//
// Created by leeiozh on 08.08.22.
//
#include <fstream>
#include "gtest/gtest.h"
#include "../src/Polygon/Matrix/Matrix.hpp"
#include "../src/Polygon/Picture/Picture.hpp"
#include "../src/Polygon/PSF/DeltaFunction/CircularDeltaFunction.hpp"
#include "../src/Polygon/PSF/DeltaFunction/SquareDeltaFunction.hpp"
#include "../src/Polygon/PSF/TransmissionFunction/ConstantTransmissionFunction.hpp"
#include "../src/Polygon/PSF/PSF.hpp"
#include "../src/Polygon/PSF/AtmosphericNoise/MoffatAtmosphericNoise.hpp"
#include "../src/Polygon/PSF/AtmosphericNoise/DiegoAtmosphericNoise.hpp"

namespace Polygon {

TEST(TEST_PSF, PSF_SIMPLE) {

    const int SIZE_X = 20;
    const int SIZE_Y = 20;

    Eigen::MatrixXd c_coeffs(2, 2);
//    c_coeffs << 0, 0, 0, 0, 0, 0;
    c_coeffs << 0.1, 0.2, 0, 0;
    Eigen::MatrixXd s_coeffs(2, 2);
    s_coeffs << 0.2, 0.1, 0, 0;
//    s_coeffs << 0, 2, 0, 1, 1, 0;

    auto aber_func_ptr = std::make_shared<AberrationFunction>(c_coeffs, s_coeffs);
    auto trans_func_ptr = std::make_shared<ConstantTransmissionFunction>(1.);
    auto delta_func_ptr = std::make_shared<CircularDeltaFunction>();

    PSF<SIZE_X, SIZE_Y> psf(aber_func_ptr, delta_func_ptr, trans_func_ptr);

//    std::cout << std::fixed << std::showpoint;
//    std::cout << std::setprecision(2);

    std::ofstream out("/home/leeiozh/mmcp/calculation_results/PSF.csv");
//    out << "x,y,psf" << std::endl;

    auto res = psf.get_matrix();

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            out << res(i, j) << ",";
        }
        out << std::endl;
    }
}

TEST(TEST_PSF, PSF_NOISE) {

    const int SIZE_X = 20;
    const int SIZE_Y = 20;

    Eigen::MatrixXd c_coeffs(2, 2);
//    c_coeffs << 0, 0, 0, 0, 0, 0;
    c_coeffs << 0, 0, 0, 0.0;
    Eigen::MatrixXd s_coeffs(2, 2);
    s_coeffs << 0, 0, 0.0, 0;
//    s_coeffs << 0, 2, 0, 1, 1, 0;

    auto aber_func_ptr = std::make_shared<AberrationFunction>(c_coeffs, s_coeffs);
    auto trans_func_ptr = std::make_shared<ConstantTransmissionFunction>(1);
    auto delta_func_ptr = std::make_shared<CircularDeltaFunction>();

    DiegoAtmosphericNoise mof_noise = DiegoAtmosphericNoise(5, 1, 1);

    PSF<SIZE_X, SIZE_Y> psf(aber_func_ptr, delta_func_ptr, trans_func_ptr);

//    std::cout << std::fixed << std::showpoint;
//    std::cout << std::setprecision(2);

    std::ofstream out("/home/leeiozh/mmcp/calculation_results/PSF.csv");
//    out << "x,y,psf" << std::endl;

    auto res = psf.get_matrix();

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            out << res(i, j) << ",";
        }
        out << std::endl;
    }
}

}