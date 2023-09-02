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

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.04/");
    ReadParameters params{0, 720, 720};
    FileReader reader(path, params);
    auto search = AreaSearch(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    auto curve = std::make_shared<DispersionCurve>(num_t, 10, 32, K_MAX);
    auto curve_vec = std::vector<std::shared_ptr<DispersionCurve>>(1);
    curve_vec[0] = curve;

    MainProcess proc(std::make_shared<FileReader>(reader), std::make_shared<AreaSearch>(search),
                     std::make_shared<Mesh>(mesh), curve_vec, true);

    std::vector<OutputStructure> res = proc.run(40);

    int i = 0;
    for (auto &r: res) {
        std::cout << r.swh[0] << " " << r.per[0] << std::endl;
        if (i % 10 == 0) {
            std::ofstream out("/home/leeiozh/ocean/seavisionCPP/freq_spec_" + std::to_string(i) + ".csv");
            for (double j: r.freq_spec) {
                out << j << ",";
            }
        }
        i += 1;
    }
}*/

/*
TEST(TEST_MAIN, DATA) {
    std::string path("/storage/tartar/DATA/RADAR/src/");
    ReadParameters params{0, AREA_SIZE, AREA_SIZE};
    auto disp_direct = DispersionDirect(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    auto curr_path = std::filesystem::current_path().string();
    curr_path = curr_path.substr(0, curr_path.size() - 5);
    std::cout << curr_path << std::endl;

    std::string curr_path = "/storage/kubrick/ezhova/SEAVISION_CPP/";

    std::string info_name(curr_path + "resources/for_cpp.csv");
    std::fstream info(info_name, std::ios::in);
    std::ofstream out(curr_path + "results/swh_per.csv");
    out << "name,m01,m02,m03,per1,per2,per3,dir1,dir2,dir3,len1,len2,len3,vcos1,vcos2,vcos3," << std::endl;

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
        OutputProcessor output("192.104.100.10", 4000);

        auto curve = std::make_shared<DispersionCurve>(FOUR_NUM, DELTA_FREQ, CUT_NUM, K_MAX);

        MainProcess proc(std::make_shared<FileReader>(reader), std::make_shared<OutputProcessor>(output),
                         std::make_shared<DispersionDirect>(disp_direct), std::make_shared<Mesh>(mesh), curve);

        OutputStructure res = proc.run_debug();

        out << files[i] << ","
            << res.m0[0] << "," << res.m0[1] << "," << res.m0[2] << ","
            << res.per[0] << "," << res.per[1] << "," << res.per[2] << ","
            << res.dir[0] << "," << res.dir[1] << "," << res.dir[2] << ","
            << res.len[0] << "," << res.len[1] << "," << res.len[2] << ","
            << res.vcos[0] << "," << res.vcos[1] << "," << res.vcos[2] << "," << std::endl;

        std::cout << files[i] << "\n"
                  << " m0 " << res.m0[0] << " " << res.m0[1] << " " << res.m0[2] << " "
                  << " per " << res.per[0] << " " << res.per[1] << " " << res.per[2] << " "
                  << " dir " << res.dir[0] << " " << res.dir[1] << " " << res.dir[2] << " "
                  << " len " << res.len[0] << " " << res.len[1] << " " << res.len[2] << " "
                  << " vcos " << res.vcos[0] << " " << res.vcos[1] << " " << res.vcos[2] << " " << std::endl;

        std::ofstream out2(curr_path + "results/rose" + std::to_string(i) + ".csv");
        for (double j: res.rose) {
            out2 << j << ",";
        }

        std::ofstream out22(curr_path + "results/freq_spec" + std::to_string(i) + ".csv");
        for (double j: res.freq_spec) {
            out22 << j << ",";
        }
    }
}
*/
} // namespace


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*namespace SeaVision {

TEST(TEST_MAIN, INFO) {
    const int num_t = FOUR_NUM;

    std::string path("/storage/tartar/DATA/RADAR/src/");

    std::string info("/home/leeiozh/ocean/seavisionCPP/SeaVision/resources/for_cpp.csv");
    std::fstream file(info, std::ios::in);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {

            std::stringstream str(line);
            std::string word;

            while (getline(str, word, ','))
                std::cout << word << std::endl;
        }
    }

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
}*/