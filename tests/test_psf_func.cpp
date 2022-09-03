//
// Created by leeiozh on 08.08.22.
//
#include <fstream>
#include "gtest/gtest.h"
#include "../src/Polygon/PSF/PSF.hpp"
#include "../src/Polygon/PSF/AtmosphericNoise/MoffatAtmosphericNoise.hpp"
#include "../src/Polygon/PSF/AtmosphericNoise/DiegoAtmosphericNoise.hpp"

namespace Polygon {

TEST(TEST_PSF, PSF_SIMPLE) {

    const int SIZE_X = 200;
    const int SIZE_Y = 200;

    Eigen::MatrixXd c_coeffs(2, 2);
//    c_coeffs << 0, 0, 0, 0, 0, 0;
    c_coeffs << 0, 0, 0, 0;
    Eigen::MatrixXd s_coeffs(2, 2);
    s_coeffs << 0, 0, 0, 0;
//    s_coeffs << 0, 2, 0, 1, 1, 0;

    auto aber_func_ptr = std::make_shared<AberrationFunction>(c_coeffs, s_coeffs);
    auto trans_func_ptr = std::make_shared<ExponentBackwardTransmissionFunction>(0.99);
    auto delta_func_ptr = std::make_shared<CircularDeltaFunction>();

    PSF<SIZE_X, SIZE_Y> psf(aber_func_ptr, delta_func_ptr, trans_func_ptr);

//    std::cout << std::fixed << std::showpoint;
//    std::cout << std::setprecision(2);

    std::ofstream out("/home/leeiozh/mmcp/calculation_results/PSF.csv");
//    out << "x,y,psf" << std::endl;

    for (int i = 0; i < SIZE_X; ++i) {
        double x = static_cast<double>(i) / SIZE_X;
        for (int j = 0; j < SIZE_Y; ++j) {
            double y = static_cast<double>(j) / SIZE_Y;
            out << psf.calc_psf(x, y) << ",";
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

    for (int i = 0; i < SIZE_X; ++i) {
        double x = static_cast<double>(i) / SIZE_X;
        for (int j = 0; j < SIZE_Y; ++j) {
            double y = static_cast<double>(j) / SIZE_Y;
            out << psf.calc_psf(x, y) * mof_noise.calc_atm_noise(x, y) << ",";
        }
        out << std::endl;
    }
}

}