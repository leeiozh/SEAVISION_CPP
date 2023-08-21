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
    ReadParameters params{0, 1024, 1024};
    auto disp_direct = DispersionDirect(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    InputProcessor inp_proc("192.168.0.103", 4000, params);
    OutputProcessor output_proc("192.168.0.102", 4000);

    auto curve = std::make_shared<DispersionCurve>(FOUR_NUM, DELTA_FREQ, CUT_NUM, K_MAX);

    MainProcess proc(std::make_shared<InputProcessor>(inp_proc),
                     std::make_shared<OutputProcessor>(output_proc),
                     std::make_shared<DispersionDirect>(disp_direct),
                     std::make_shared<Mesh>(mesh), curve);

    proc.run_realtime();
}

TEST(TEST_PARALLEL, CONNECT) {

    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07/");

    ReadParameters params{0, 4096, 4096};
    FileReader reader(path, params);

    Sender sender("192.168.0.102", 4000, std::make_shared<FileReader>(reader));

    std::thread send_thread([&]() {
        sender.pass_queue_files(path, 300);
    });

    std::thread receive_thread([&]() {

        struct sockaddr_in server_address{};
        std::string ip; // IP of current machine
        int port; // port for receiving data
        int socket_descriptor; // internal stuff

        socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_descriptor == -1) {
            std::stringstream buff;
            buff << "Error when creating a socket!";
            throw SeaVisionException(buff.str().c_str());
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

        if (bind(socket_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
            std::stringstream buff;
            buff << "Error when binding a socket!";
            throw SeaVisionException(buff.str().c_str());
        }

        int counter = 0;

        while (true) {

            unsigned char first_byte;
            recv(socket_descriptor, &first_byte, sizeof(first_byte), 0);

            if (first_byte == 0x5) {

                std::cout << "received parcel number " << counter << std::endl;

                uint16_t data[12];
                recv(socket_descriptor, &data, sizeof(data), 0);

                for (auto &i: data) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;

                uint8_t rose[NUM_AREA];
                recv(socket_descriptor, &rose, sizeof(rose), 0);

                std::ofstream file_rose(
                        "/home/leeiozh/ocean/seavisionCPP/SeaVision/tests/rose/rose" + std::to_string(counter) + ".csv");
                for (auto &i : rose){
                    file_rose << i << ",";
                }
                counter++;
            }
        }
    });

    send_thread.join();
    receive_thread.join();
}

} // namespace