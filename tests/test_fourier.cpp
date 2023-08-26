//
// Created by leeiozh on 6/13/23.
//

#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/Structures.hpp"
#include "gtest/gtest.h"

const double TOLERANCE = 1.;

namespace SeaVision {
TEST(TEST_FOURIER, FOURIER) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(1)[0];

    Mesh mesh = Mesh(params, STEP);
    Area area = Area(720, 720, 0, 0, 0);

    Eigen::MatrixXd res_back = mesh.calc_back(area, res_inp.prli.bcksctr);

    DispersionCurve dispersionCurve(256, 15, 32, K_MAX);
    Eigen::MatrixXcd res = dispersionCurve.calc_fourier_2d_one(res_back);

    ASSERT_FALSE(std::abs(norm(res(0, 0)) - 360117451380611.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(norm(res(0, 1)) - 49899532621.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(norm(res(1, 0)) - 243988643012.0) > TOLERANCE);
}
}

namespace SeaVision {
TEST(TEST_WELCH, WELCH) {

    const int num_t = 256;

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07_1200/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(num_t);

    Mesh mesh = Mesh(params, STEP);
    Area area = Area(720, 720, 0, 0, 840);

    DispersionCurve dispersionCurve(num_t, 15, 32, K_MAX);

    Eigen::VectorX<Eigen::MatrixXd> res_back(num_t);
    for (int i = 0; i < num_t; ++i) {
        res_back[i] = mesh.calc_back(area, res_inp[i].prli.bcksctr);
        dispersionCurve.update(i, res_back[i]);
    }

    auto wel = dispersionCurve.calc_welch(num_t - 1);
    auto res = DispersionCurve::calc_abs_wave_num(wel);

    ASSERT_FALSE(std::abs(res(0, 0) - 15562655215635.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(0, 1) - 52843662225296.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(1, 0) - 41702768062952.0) > TOLERANCE);
}

} // namespace



