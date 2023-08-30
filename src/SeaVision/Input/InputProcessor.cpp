//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include "InputProcessor.hpp"

namespace SeaVision {

InputProcessor::InputProcessor(int prli_port, int navi_port, const ReadParameters &params) : params_read(params) {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw SeaVisionException("Failed to initialize Winsock.");


    params_prli.socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (params_prli.socket_descriptor == INVALID_SOCKET) {
        WSACleanup();
        throw SeaVisionException("Failed to create socket.");
    }

    params_prli.server_address.sin_family = AF_INET;
    params_prli.server_address.sin_port = htons(prli_port);
    params_prli.server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(params_prli.socket_descriptor, (struct sockaddr *) &params_prli.server_address,
             sizeof(params_prli.server_address)) == SOCKET_ERROR) {
        closesocket(params_prli.socket_descriptor);
        WSACleanup();
        throw SeaVisionException("Error binding socket for PRLI. Input port >> " + std::to_string(prli_port));
    }

    params_navi.socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (params_navi.socket_descriptor == INVALID_SOCKET) {
        WSACleanup();
        throw SeaVisionException("Failed to create socket.");
    }

    params_navi.server_address.sin_family = AF_INET;
    params_navi.server_address.sin_port = htons(navi_port);
    params_navi.server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(params_navi.socket_descriptor, (struct sockaddr *) &params_navi.server_address,
             sizeof(params_navi.server_address)) == SOCKET_ERROR) {
        closesocket(params_navi.socket_descriptor);
        WSACleanup();
        throw SeaVisionException("Error binding socket for navigation. Input port >> " + std::to_string(navi_port));
    }

    curr_prli.bcksctr.resize(params.line_size, params.size_angle);
    curr_prli.bcksctr.setZero();

    start_part = params.line_start / 1024;
    finish_part = params.line_end / 1024;
}

bool InputProcessor::listen_navi() {

    unsigned char data[20];

    ssize_t bytesRead = recv(params_navi.socket_descriptor, reinterpret_cast<char *>(data), sizeof(data), 0);
    if (bytesRead == -1 or static_cast<int>(data[0]) != 1)
        throw SeaVisionException("Error when reading a navigation parameters!");

    double speed_lag = static_cast<double>((((int16_t) data[5]) << 8) | data[4]) / 100.;
    double gyro = static_cast<double>((((uint16_t) data[7]) << 8) | data[6]) / 100.;

    double lat = static_cast<double>(((int32_t) data[11] << 24) | ((int32_t) data[10] << 16)
                                     | ((int32_t) data[9] << 8) | ((int32_t) data[8])) / 1000000.;

    double lon = static_cast<double>(((int32_t) data[15] << 24) | ((int32_t) data[14] << 16)
                                     | ((int32_t) data[13] << 8) | ((int32_t) data[12])) / 1000000.;

    double cog = static_cast<double>((((uint16_t) data[17]) << 8) | data[16]) / 100.;

    if (lat < -90. or lat > 90. or lon < -180. or lon > 180. or cog < 0. or cog > 360.) return false;

    double sog = static_cast<double>((((uint16_t) data[19]) << 8) | data[18]) / 100.;

    curr_navi = InputNavi{cog, sog, lat, lon, speed_lag, gyro};

    return true; // only if the result is adequate
}

int InputProcessor::listen_prli() {

    unsigned char pack[1032];

    size_t bytesRead = recv(params_prli.socket_descriptor, reinterpret_cast<char *>(pack), sizeof(pack), 0);
    if (bytesRead == -1 or static_cast<int>(pack[0]) != 8)
        throw SeaVisionException("Error when reading a PRLI!");

    int num_line = static_cast<int>((((unsigned short) pack[2]) << 8) | (unsigned short) pack[1]);

    double step = static_cast<double>((((unsigned short) pack[4]) << 8) | (unsigned short) pack[3]) / 1000.;

    if (step != 1.875 and step != 3.75 and step != 7.5 and step != 15. and step != 30.)
        throw SeaVisionException("Incorrect step!!! Input step >> " + std::to_string(step));

    curr_prli.step = step;

    int num_part = static_cast<int>(pack[5]) - 1;

    int num_all = static_cast<int>(pack[6]);

    curr_prli.pulse = static_cast<int>(pack[7]);

    if (num_part >= start_part and num_part < finish_part) {
        for (int i = 0; i < 1024; ++i) {
            curr_prli.bcksctr(i - params_read.line_start + num_part * 1024, num_line) = static_cast<int>(pack[i + 8]);
        }
    }

    if (num_part == 0) {
        if (ready_vec(num_line)) double_counter++; // finish filling if we go fourth time on one place
        else ready_vec(num_line) = true;           // think that line with this number is ready
    }

    if (double_counter >= 4) return static_cast<int>(ready_vec.size());

    return ready_vec.cast<int>().sum();
}

InputStructure InputProcessor::listen_message() {

    double_counter = 0;
    int prli_ready = 0.;        // number of ready lines
    bool navi_ready = false;    // flag if conditions are ready
    ready_vec.setZero();        // zeroing vector of readiness

    const auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    double elapsed_seconds;     // timer for check how long this process go

    while (prli_ready < ready_vec.size()) { // loop while backscatter not fill completely and conditions read

        try {

            prli_ready = listen_prli();
            if (prli_ready >= ready_vec.size())
                std::cout << "PRLI received: " << ready_vec.cast<int>().sum() * 100. / ready_vec.size() << "%"
                          << std::endl;

            if (!navi_ready) {
                navi_ready = listen_navi();
                if (navi_ready) {
                    std::cout << "Navigation received: lat = " << curr_navi.lat << ", lon = " << curr_navi.lon
                              << ", sog = " << curr_navi.sog << ", cog = " << curr_navi.cog << ", speed = "
                              << curr_navi.spd << ", gyro = " << curr_navi.hdg << " " << std::endl;
                }
            }

        } catch (const SeaVisionException &exception) {
            std::cerr << "Local read error: " << exception.what() << std::endl;
            continue;
        }

        end = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration<double>(end - start).count();

        if (elapsed_seconds > MAX_WAIT_SHOT_TIME) { // if process go more time than expected
            if (prli_ready < 0.9 * static_cast<double >(ready_vec.size())) {
                std::stringstream buff;
                buff << "Too much time for reading PRLI!";
                throw SeaVisionException(buff.str().c_str());
            } else { // but we admit that the part (90%) of backscatter can be filled
                break;
            }
        }
    }
    return InputStructure{curr_navi, curr_prli};
}

InputProcessor::~InputProcessor() {
    close(params_prli.socket_descriptor);
    close(params_navi.socket_descriptor);
    WSACleanup();
}

} // namespace