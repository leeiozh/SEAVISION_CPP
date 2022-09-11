//
// Created by leeiozh on 03.09.22.
//

#include <fstream>
#include "gtest/gtest.h"
#include "../src/Polygon/Matrix/Matrix.hpp"
#include "../src/Polygon/Picture/Picture.hpp"
#include "../src/Polygon/VisualField/CircleVisualField.hpp"
#include "../src/Polygon/PSF/DeltaFunction/CircularDeltaFunction.hpp"
#include "../src/Polygon/PSF/DeltaFunction/SquareDeltaFunction.hpp"
#include "../src/Polygon/PSF/TransmissionFunction/ConstantTransmissionFunction.hpp"
#include "../src/Polygon/PSF/AtmosphericNoise/MoffatAtmosphericNoise.hpp"

namespace Polygon {

TEST(TEST_PICTURE, PICTURE_SIMPLE) {

    const int SIZE_X = 1000;
    const int SIZE_Y = 1000;

    Polygon::NoiseParams noiseParams{3, 3};
    const Polygon::MatrixParams matrixParams{1e-5, 1e-5, 1e-5 * SIZE_X, 1e-5 * SIZE_Y, noiseParams, 100000};

    std::shared_ptr<Polygon::BaseVisualField> field = std::make_shared<Polygon::CircleVisualField>(2. / deg,
                                                                                                   Eigen::Vector2d(
                                                                                                           matrixParams.matr_x,
                                                                                                           matrixParams.matr_y),
                                                                                                   "/home/leeiozh/mmcp/polygon/Polygon/resources");

    Eigen::Vector3d scope_dir = {1 / std::sqrt(2), -1 / std::sqrt(2), 0};
    Eigen::Vector3d scope_pos = {6370e3, 0, 0};
    ScopeState scope_state = {scope_pos, scope_dir};
    Eigen::Vector3d sun_pos = {-1.496e11, 0, 0.};

    SatState sat = {1, 0.5, {6770e3, 0, 0}};
    std::vector<Polygon::SatState> sat_state(1);
    sat_state[0] = sat;

    Eigen::MatrixXd c_coeffs(2, 2);
    c_coeffs << 0., 0.5, 0, 0;
    Eigen::MatrixXd s_coeffs(2, 2);
    s_coeffs << 0., 0.5, 0, 0;

    auto aber_func_ptr = std::make_shared<AberrationFunction>(c_coeffs, s_coeffs);
    auto trans_func_ptr = std::make_shared<ConstantTransmissionFunction>(0.95);
    auto delta_func_ptr = std::make_shared<CircularDeltaFunction>();

    auto atm_noise = std::make_shared<MoffatAtmosphericNoise>(1.e-2, 0.5);

    PSF<SIZE_X, SIZE_Y> psf(aber_func_ptr, delta_func_ptr, trans_func_ptr, atm_noise);
    SkyLight<SIZE_X, SIZE_Y> sky(1e-11);

    std::array<double, COLOR_NUMBER> lengths = {700.}; //, 546.1, 435.8};
    std::array<double, COLOR_NUMBER> quant_eff = {1.}; //, 1., 1.};

    Matrix<SIZE_X, SIZE_Y> matrix(matrixParams, lengths, quant_eff);

    Polygon::Picture<SIZE_X, SIZE_Y> picture(matrix, psf, field, sky);

    auto res = picture.calc_pictures(scope_state, sat_state, sun_pos, 1e-10);

    std::cout << res[0](150, 150) << std::endl;

    std::ofstream out("/home/leeiozh/mmcp/calculation_results/Picture.csv");

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            out << res[0](i, j) << ",";
        }
        out << std::endl;
    }
}
}