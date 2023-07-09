//
// Created by leeiozh on 7/8/23.
//

#include <fstream>
#include "gtest/gtest.h"
#include "SeaVision/MainProcess/MainProcess.hpp"

namespace SeaVision {

TEST(TEST_MAIN, MAIN) {

    const int num_t = FOUR_NUM;

    std::string path("/home/leeiozh/ocean/seavisionCPP/0606_4338/");
    ReadParameters params{0, 720, 720};
    FileReader reader(path, params);
    auto search = AreaSearch(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    auto curve = std::make_shared<DispersionCurve>(num_t, 10, 32, K_MAX);
    auto curve_vec = std::vector<std::shared_ptr<DispersionCurve>>(1);
    curve_vec[0] = curve;

    MainProcess proc(std::make_shared<FileReader>(reader), std::make_shared<AreaSearch>(search),
                     std::make_shared<Mesh>(mesh), curve_vec);

    std::vector<OutputStructure> res = proc.run(60);

    int i = 0;
    for (auto &r: res) {
        std::cout << r.swh[0] << " " << r.per[0] << std::endl;

        if (i % 10 == 0) {
            std::ofstream out("/home/leeiozh/ocean/seavisionCPP/freq" + std::to_string(i) + ".csv");
            for (double j: r.freq_spec) {
                out << j << ",";
            }
        }
        i += 1;
    }
}
}