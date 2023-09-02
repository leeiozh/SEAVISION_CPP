//
// Created by leeiozh on 8/15/23.
//

#include <filesystem>
#include <fstream>
#include "gtest/gtest.h"
#include <winsock2.h>
#include "SeaVision/MainProcess/MainProcess.hpp"
#include "SeaVision/Output/Sender.hpp"

namespace SeaVision {

TEST(TEST_PARALLEL, DATA) {
    std::string path("/home/leeiozh/ocean/seavisionCPP/2022.10.07/");
    ReadParameters params{0, 1024, 1024};
    auto disp_direct = DispersionDirect(NUM_AREA);
    Mesh mesh = Mesh(params, STEP);

    InputProcessor inp_proc(4000, params);
    OutputProcessor output_proc("192.168.0.102", 4000);

    auto curve = std::make_shared<DispersionCurve>(FOUR_NUM, DELTA_FREQ, CUT_NUM, K_MAX);

    MainProcess proc(std::make_shared<InputProcessor>(inp_proc),
                     std::make_shared<OutputProcessor>(output_proc),
                     std::make_shared<DispersionDirect>(disp_direct),
                     std::make_shared<Mesh>(mesh), curve);

    proc.run_realtime();
}

TEST(TEST_PARALLEL, CONNECT) {

    std::string path("/home/leeiozh/ocean/SEAVISION_CPP/2022.10.07/");

    ReadParameters params{0, 4096, 4096};
    FileReader reader(path, params);

    Sender sender("192.168.0.103", 4000, std::make_shared<FileReader>(reader));

    std::thread send_thread([&]() {
        sender.pass_queue_files(path, 300);
    });

    std::thread receive_thread([&]() {

        sockaddr_in serverAddress;
        std::string ip = "192.168.0.102"; // IP of current machine
        int portNumber = 4000; // port for receiving data
        SOCKET socketDescriptor; // internal stuff
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Failed to initialize Winsock.");
        }

        socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketDescriptor == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Failed to create socket.");
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(portNumber);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (bind(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            closesocket(socketDescriptor);
            WSACleanup();
            throw std::runtime_error("Error binding socket.");
        }

        int counter = 0;

        while (true) {

            unsigned char first_byte;
            recv(socketDescriptor, reinterpret_cast<char *>(&first_byte), sizeof(first_byte), 0);

            if (first_byte == 0x5) {

                std::cout << "received parcel number " << counter << std::endl;

                uint16_t data[12];
                recv(socketDescriptor, reinterpret_cast<char *>(&data), sizeof(data), 0);

                for (auto &i: data) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;

                uint8_t rose[NUM_AREA];
                recv(socketDescriptor, reinterpret_cast<char *>(&rose), sizeof(rose), 0);

                std::ofstream file_rose(
                        "/home/leeiozh/ocean/SEAVISION_CPP/tests/rose/rose" + std::to_string(counter) + ".csv");
                for (auto &i: rose) {
                    file_rose << static_cast<int>(i) << ",";
                }
                counter++;
            }
        }
    });

    send_thread.join();
    receive_thread.join();
}

} // namespace