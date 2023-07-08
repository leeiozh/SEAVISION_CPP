//
// Created by leeiozh on 6/13/23.
//

#include <fstream>
#include <chrono>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/FourierStage/SpectrumStructure.hpp"
#include "SeaVision/Interpolant/AreaSearch.hpp"
#include "gtest/gtest.h"

const double TOLERANCE = 1.;

namespace SeaVision {
TEST(TEST_FOURIER, FOURIER) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(1)[0];

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, 0, 0, 0);

    Eigen::MatrixXd res_back = mesh.calc_back(area, res_inp.bcksctr);

    DispersionCurve dispersionCurve(256, 15, 32, K_MAX);
    Eigen::MatrixXcd res = dispersionCurve.calc_fourier_2d_one(res_back);

    ASSERT_FALSE(std::abs(norm(res(0, 0)) - 360117451380611.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(norm(res(0, 1)) - 49899532621.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(norm(res(1, 0)) - 243988643012.0) > TOLERANCE);

    /*std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four_real.csv");
    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << norm(res(i, j)) << ",";
            // std::cout << i << " " << j << " " << res(i, j).real() << " " << res(i, j).imag()<<std::endl;
        }
        out2 << std::endl;
    }*/
}
}

namespace SeaVision {
TEST(TEST_WELCH, WELCH) {

    const int num_t = 256;

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07_1200/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(num_t);

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, 0, 0, 840);

    DispersionCurve dispersionCurve(num_t, 15, 32, K_MAX);

    Eigen::VectorX<Eigen::MatrixXd> res_back(num_t);
    for (int i = 0; i < num_t; ++i) {
        res_back[i] = mesh.calc_back(area, res_inp[i].bcksctr);
        dispersionCurve.update(i, res_back[i]);
    }

    auto wel = dispersionCurve.calc_welch();
    auto res = DispersionCurve::calc_abs_wave_num(wel);

    ASSERT_FALSE(std::abs(res(0, 0) - 16080154463906.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(0, 1) - 54965824852787.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(1, 0) - 42021925559540.0) > TOLERANCE);

    /*std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four_real.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j) << ",";
            // std::cout << i << " " << j << " " << res(i, j).real() << " " << res(i, j).imag()<<std::endl;
        }
        out2 << std::endl;
    }*/
}
}

namespace SeaVision {
TEST(TEST_CURVE, CURVE) {

    const int num_t = 256;

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07_1200/");
    ReadParameters params{0, 720, 720};
    FileReader reader(path, params);

    auto start = std::chrono::steady_clock::now();
    auto res_inp = reader.read_queue_files(num_t);
    auto search = AreaSearch(18);
    std::vector<InputStructure> std_back = std::vector<InputStructure>(res_inp.begin(), res_inp.begin() + 4);
    double std_ang = search.search_area(std_back);
    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, -std_ang, std_ang, 840); // 840 (minus, plus)

    DispersionCurve dispersionCurve(num_t, 15, 32, K_MAX);
    Eigen::VectorX<Eigen::MatrixXd> res_back(num_t);

    for (int i = 0; i < num_t; ++i) {
        res_back[i] = mesh.calc_back(area, res_inp[i].bcksctr);
        dispersionCurve.update(i, res_back[i]);
    }

    SpectrumStruct res = dispersionCurve.get_params();
    std::cout << "m0 " << res.m0 << "; period " << res.peak_period << std::endl;

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/freq_spec.csv");
    for (double j: res.freq_spec) {
        out << j << ",";
    }
}
} // namespace



