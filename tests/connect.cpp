//
// Created by leeiozh on 23.08.2023.
//

#include "SeaVision/Input/ConfigReader.hpp"
#include "SeaVision/MainProcess/MainProcess.hpp"

using namespace SeaVision;

int main() {

    char result[MAX_PATH];
    std::string curr_path = std::string(result, GetModuleFileName(nullptr, result, MAX_PATH));

    try {

        ConfigReader config(curr_path.substr(0, curr_path.size() - 15) + "config.txt");

        std::cout << "config read successfully" << std::endl;

        ReadParameters params{0, 1024, 1024, 4096};
        auto disp_direct = DispersionDirect(NUM_AREA);

        Mesh mesh = Mesh(params);

        std::cout << "mesh creates successfully" << std::endl;

        InputProcessor inp_proc(config.get_prli_port(), config.get_navi_port(), params);
        OutputProcessor output_proc(config.get_ip(), config.get_serv_port());

        auto curve = DispersionCurve();

        MainProcess proc(std::make_unique<InputProcessor>(inp_proc),
                         std::make_unique<OutputProcessor>(output_proc),
                         std::make_unique<DispersionDirect>(disp_direct),
                         std::make_unique<Mesh>(mesh),
                         std::make_unique<DispersionCurve>(curve), FOUR_OR_DISP);

        proc.run_realtime();

    }
    catch (const SeaVisionException &exception) {
        std::cerr << "Preparing error: " << exception.what() << std::endl;
        std::cout << "Automatic exit after 10 sec..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;

}