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

namespace SeaVision {

TEST(TEST_FOURIER, FOURIER) {

    Eigen::MatrixXd data = Eigen::MatrixXd::Zero(512, 512);

    for (int i = 0; i < data.cols(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            data(i, j) = std::cos(M_PI * i / data.cols() * 10) + std::cos(M_PI * i / data.cols() * 20);
        }
    }

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_data1.csv");

    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            out << data(i, j) << ",";
        }
        out << std::endl;
    }

    DispersionCurve dispersionCurve(256, 15, data.cols(), K_MAX);

    Eigen::MatrixXcd res = dispersionCurve.calc_fourier_2d_one(data);

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four1.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j).real() << ",";
        }
        out2 << std::endl;
    }
}
}

namespace SeaVision {
TEST(TEST_FOURIER_INP, FOURIER_INP) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(1)[0];

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, 0, 0, 0);

    Eigen::MatrixXd res_back = mesh.calc_back(area, res_inp.bcksctr);

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_back.csv");

    for (int i = 0; i < res_back.rows(); ++i) {
        for (int j = 0; j < res_back.cols(); ++j) {
            out << res_back(i, j) << ",";
        }
        out << std::endl;
    }

    DispersionCurve dispersionCurve(256, 15, 32, K_MAX);
    Eigen::MatrixXcd res = dispersionCurve.calc_fourier_2d_one(res_back);

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four_real.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << norm(res(i, j)) << ",";
            // std::cout << i << " " << j << " " << res(i, j).real() << " " << res(i, j).imag()<<std::endl;
        }
        out2 << std::endl;
    }

}
}

namespace SeaVision {
TEST(TEST_WELCH, WELCH) {

    const int num_t = 256;

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07_1200/");
    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto start = std::chrono::steady_clock::now();

    auto res_inp = reader.read_queue_files(num_t);

    auto end = std::chrono::steady_clock::now();
    std::cout << "reading " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, 0, 0, 840);

    end = std::chrono::steady_clock::now();
    std::cout << "meshing " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    DispersionCurve dispersionCurve(num_t, 15, 32, K_MAX);

    Eigen::VectorX<Eigen::MatrixXd> res_back(num_t);
    for (int i = 0; i < num_t; ++i) {
        res_back[i] = mesh.calc_back(area, res_inp[i].bcksctr);
        dispersionCurve.update(i, res_back[i]);
    }

    end = std::chrono::steady_clock::now();
    std::cout << "back " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    auto wel = dispersionCurve.calc_welch();

    end = std::chrono::steady_clock::now();
    std::cout << "welch " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;


    auto res = DispersionCurve::calc_abs_wave_num(wel);

    end = std::chrono::steady_clock::now();
    std::cout << "abs " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_four_real.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j) << ",";
            // std::cout << i << " " << j << " " << res(i, j).real() << " " << res(i, j).imag()<<std::endl;
        }
        out2 << std::endl;
    }

}

}

namespace SeaVision {
TEST(TEST_CURVE, CURVE) {

    const int num_t = 256;

    std::string path("/home/leeiozh/ocean/seavisionCPP/0606_4338/");

    ReadParameters params{0, 720, 720};

    FileReader reader(path, params);

    auto start = std::chrono::steady_clock::now();

    auto res_inp = reader.read_queue_files(num_t);

    auto end = std::chrono::steady_clock::now();
    std::cout << "reading " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    auto search = AreaSearch(18);
    std::vector<InputStructure> std_back = std::vector<InputStructure>(res_inp.begin(), res_inp.begin() + 4);
    double std_ang = search.search_area(std_back);

    std::cout << std_ang << std::endl;

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, -std_ang, std_ang, 840); // 840 (minus, plus)

    end = std::chrono::steady_clock::now();
    std::cout << "meshing " << std::chrono::duration<double>(end - start).count() << "s\n";
    start = end;

    DispersionCurve dispersionCurve(num_t, 15, 32, K_MAX);
    Eigen::VectorX<Eigen::MatrixXd> res_back(num_t);

    for (int i = 0; i < num_t; ++i) {

        res_back[i] = mesh.calc_back(area, res_inp[i].bcksctr);
        dispersionCurve.update(i, res_back[i]);

        /*
        if (i == 0) {
            std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_back.csv");

            for (int ii = 0; ii < res_back[i].rows(); ++ii) {
                for (int j = 0; j < res_back[i].cols(); ++j) {
                    out << res_back[i](ii, j) << ",";
                }
                out << std::endl;
            }
        }*/
    }

    SpectrumStruct res = dispersionCurve.get_params();

    std::cout << res.m0 << " " << res.peak_period << std::endl;

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/freq_spec.csv");

    for (double j : res.freq_spec) {
        out << j << ",";
    }
}


} // namespace



