//
// Created by leeiozh on 7/8/23.
//
#include <filesystem>
#include <fstream>
#include "gtest/gtest.h"
#include "SeaVision/MainProcess/MainProcess.hpp"

namespace SeaVision {

/*TEST(TEST_MAIN, MAIN) {

    const int num_t = FOUR_NUM;

    std::string path("/storage/tartar/DATA/RADAR/src/2022.10.09/");
    ReadParameters params{0, 720, 720};
    FileReader reader(path, params);
    auto search = AreaSearch(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    auto curve = std::make_shared<DispersionCurve>(num_t, 10, 32, K_MAX);
    auto curve_vec = std::vector<std::shared_ptr<DispersionCurve>>(1);
    curve_vec[0] = curve;

    MainProcess proc(std::make_shared<FileReader>(reader), std::make_shared<AreaSearch>(search),
                     std::make_shared<Mesh>(mesh), curve_vec);

    std::vector<OutputStructure> res = proc.run(40);

    int i = 0;
    for (auto &r: res) {
        std::cout << r.swh[0] << " " << r.per[0] << std::endl;

        if (i % 10 == 0) {
            std::ofstream out("/storage/kubrick/ezhova/seavisionCPP/SeaVision/results/freq_spec_" + std::to_string(i) + ".csv");
            for (double j: r.freq_spec) {
                out << j << ",";
            }
        }
        i += 1;
    }
}*/


TEST(TEST_MAIN, DATA) {
    const int num_t = FOUR_NUM;
    std::string path("/storage/tartar/DATA/RADAR/src/");
    ReadParameters params{0, 720, 720};
    auto search = AreaSearch(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    /*auto curr_path = std::filesystem::current_path().string();
    curr_path = curr_path.substr(0, curr_path.size() - 5);
    std::cout << curr_path << std::endl;*/

    std::string curr_path = "/storage/kubrick/ezhova/SEAVISION_CPP/";

    std::string info_name(curr_path + "resources/for_cpp.csv");
    std::fstream info(info_name, std::ios::in);
    std::ofstream out(curr_path + "results/swh_per.csv");
    out << "m0,per," << std::endl;

    std::vector<std::string> files;
    bool start = true;
    if (info.is_open()) {
        std::string line;
        while (getline(info, line)) {
            if (start) {
                start = false;
            } else {
                std::stringstream str(line);
                std::string word;
                int counter = 0;
                while (getline(str, word, ',')) {
                    counter++;
                    if (counter == 2) {
                        files.push_back(word);
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < files.size(); ++i) { // files.size()

        std::string date = files[i].substr(0, 4);// + "." + files[i].substr(4, 6);
        date += ".";
        date += files[i].substr(4, 2);
        date += ".";
        date += files[i].substr(6, 2);
        date += "/";
        std::cout << "folder " << date << std::endl;

        FileReader reader(path + date, params);

        auto curve = std::make_shared<DispersionCurve>(num_t, 10, 32, K_MAX);
        auto curve_vec = std::vector<std::shared_ptr<DispersionCurve>>(1);
        curve_vec[0] = curve;

        MainProcess proc(std::make_shared<FileReader>(reader), std::make_shared<AreaSearch>(search),
                         std::make_shared<Mesh>(mesh), curve_vec, false);

        OutputStructure res = proc.run(files[i]);


        out << res.m0 << "," << res.per[0] << std::endl;
        std::cout << res.m0 << " " << res.per[0] << std::endl;

        std::ofstream out2(curr_path + "results/freq_spec" + std::to_string(i) + ".csv");
        for (double j: res.freq_spec) {
            out2 << j << ",";
        }
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}