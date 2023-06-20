//
// Created by leeiozh on 6/13/23.
//

#include <fstream>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "gtest/gtest.h"

namespace SeaVision {

TEST(TEST_FILE_READER, READER) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");

    FileReader reader(path);

    auto res_inp = reader.read_queue_files(1)[0];

    std::ofstream out("/home/leeiozh/ocean/seavisionCPP/test_back.csv");

    for (int i = 0; i < res_inp.bcksctr.rows(); ++i) {
        for (int j = 0; j < res_inp.bcksctr.cols(); ++j) {
            out << res_inp.bcksctr(i, j) << ",";
        }
        out << std::endl;
    }

    Mesh mesh = Mesh(4096, 4096, 1.875);
    Area area = Area(720, 720, 0, 0, 840);
    Eigen::MatrixXd res = mesh.calc_back(area, res_inp.bcksctr);

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_dec1.csv");

    for (int i = 0; i < res.rows(); ++i) {
        for (int j = 0; j < res.cols(); ++j) {
            out2 << res(i, j) << ",";
        }
        out2 << std::endl;
    }

}
}