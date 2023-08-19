//
// Created by leeiozh on 8/15/23.
//

#include <filesystem>
#include <fstream>
#include "gtest/gtest.h"
#include "SeaVision/MainProcess/MainProcess.hpp"
#include "SeaVision/Output/Sender.hpp"

namespace SeaVision {

TEST(TEST_PARALLEL, DATA) {
    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07/");
    ReadParameters params{0, AREA_SIZE, AREA_SIZE};
    auto disp_direct = DispersionDirect(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    InputProcessor inp_proc("192.100", 5000, params);
    OutputProcessor output_proc("192.100", 5000);

    auto curve = std::make_shared<DispersionCurve>(FOUR_NUM, DELTA_FREQ, CUT_NUM, K_MAX);

    MainProcess proc(std::make_shared<InputProcessor>(inp_proc),
                     std::make_shared<OutputProcessor>(output_proc),
                     std::make_shared<DispersionDirect>(disp_direct),
                     std::make_shared<Mesh>(mesh), curve);

    proc.run_realtime();
}

TEST(TEST_PARALLEL, CONNECT){

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07/");

    ReadParameters params{0, 4096, 4096};
    FileReader reader(path, params);

    Sender sender("192.168.0.102", 4000, std::make_shared<FileReader>(reader));

    sender.pass_queue_files(path, 4);
}

} // namespace