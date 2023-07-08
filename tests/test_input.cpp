//
// Created by leeiozh on 6/13/23.
//

#include <fstream>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "gtest/gtest.h"

const double TOLERANCE = 1e-13;

namespace SeaVision {

TEST(TEST_FILE_READER, READER) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/0606_4338/");
    ReadParameters params{0, 4096, 4096};

    FileReader reader(path, params);

    auto res_inp = reader.read_queue_files(256)[0];

    Mesh mesh = Mesh(params, 1.875);
    Area area = Area(720, 720, 0, 0, 0);
    Eigen::MatrixXd res = mesh.calc_back(area, res_inp.bcksctr);

    ASSERT_FALSE(std::abs(res(0, 0) - 177.0) > TOLERANCE);
    ASSERT_FALSE(std::abs(res(100, 100) - 116.0) > TOLERANCE);

    /*std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_back_cart.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j) << ",";
        }
        out2 << std::endl;
    }*/

}
}