//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include <fstream>
#include "InputProcessor.hpp"

namespace SeaVision {

    InputProcessor::InputProcessor(int port, const ReadParameters &params) : port(port), params(params) {

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw SeaVisionException("Failed to initialize Winsock.");
        }

        socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_descriptor == INVALID_SOCKET) {
            WSACleanup();
            throw SeaVisionException("Failed to create socket.");
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = INADDR_ANY;

        if (bind(socket_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) == SOCKET_ERROR) {
            closesocket(socket_descriptor);
            WSACleanup();
            throw SeaVisionException("Error binding socket.");
        }

        ready_vec.resize(params.size_angle); // resizing vector of readiness
        curr_prli.bcksctr.resize(params.line_size, params.size_angle);
        curr_prli.bcksctr.setZero();

        start_part = params.line_start / 1024;
        finish_part = params.line_end / 1024;
        double_counter = 0;
    }

    unsigned char InputProcessor::listen_first_byte() const {

        unsigned char first_byte;
        ssize_t bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(&first_byte), sizeof(first_byte), 0);

        if (bytesRead == -1) throw SeaVisionException("Error when reading a first byte!");

        return first_byte;
    }

    bool InputProcessor::listen_conditions() {

        int32_t lat_lon[2];
        ssize_t bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(lat_lon), sizeof(lat_lon), 0);

        if (bytesRead == -1) throw SeaVisionException("Error when reading a latitude and longitude!");

        double lat = static_cast<double>(lat_lon[0]) / 1000000.;
        double lon = static_cast<double>(lat_lon[1]) / 1000000.;

        // adequacy check
        if (lat < -180. or lat > 180. or lon < -90. or lon > 90.) return false;

        uint16_t cshs[4];
        bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(cshs), sizeof(cshs), 0);

        if (bytesRead == -1) throw SeaVisionException("Error when reading a cog, sog, heading or speed!");

        double cog = static_cast<double>(cshs[0]) / 100.;
        double sog = static_cast<double>(cshs[1]) / 100.;
        double hdg = static_cast<double>(cshs[2]) / 100.;
        double spd = static_cast<double>(cshs[3]) / 100.;

        // adequacy check
        if (cog < 0. or cog > 360. or sog < 0. or sog > 1000. or hdg < 0. or hdg > 360. or spd < 0. or spd > 1000.)
            return false;

        curr_cond = InputConditions{cog, sog, lat, lon, spd, hdg};

        return true; // only if the result is adequate
    }

    int InputProcessor::listen_prli() {

        unsigned short num_step[2];
        ssize_t bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(num_step), sizeof(num_step), 0);

        int num = static_cast<int>(num_step[0]); // number of received line (from 0 to 4096)

        curr_prli.step = static_cast<double>(num_step[1]) / 1000.; // distance in meters between neighbour cells

        if (bytesRead == -1 or num >= 4096 or num < 0 or curr_prli.step < 1.)  // adequacy check
            throw SeaVisionException("Error when reading a line number or step!");


        unsigned char part_all[3]; // second byte is 4, third byte reserved
        bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(part_all), sizeof(part_all), 0);

        int part = static_cast<int>(part_all[0]); // number of line part (from 0 to 4)

        if (bytesRead == -1 or part < 0 or part > 4) { // adequacy check
            std::stringstream buff;
            buff << "Error when reading a number of line part! Read part >> " << part << "!";
            throw SeaVisionException(buff.str().c_str());
        }

        unsigned char line[1024];
        bytesRead = recv(socket_descriptor, reinterpret_cast<char *>(line), sizeof(line), 0);

        if (bytesRead == -1) throw SeaVisionException("Error when reading a PRLI!");


        if (part >= start_part and part < finish_part) {
            for (int i = 0; i < 1024; ++i) {
                curr_prli.bcksctr(i - params.line_start + part * i, num) = static_cast<int>(line[i]);
            }
        }

        if (part == 0) {
            if (ready_vec(num)) double_counter++; // finish filling if we go second time
            else ready_vec(num) = true; // think that line with this number is ready
        }
        if (double_counter >= 4) return static_cast<int>(ready_vec.size());

        return ready_vec.cast<int>().sum();
    }

    InputStructure InputProcessor::listen_message() {

        double_counter = 0;
        int prli_ready = 0.; // number of ready lines
        bool cond_ready = false; // flag if conditions are ready
        ready_vec = Eigen::VectorX<bool>::Zero(params.size_angle); // zeroing vector of readiness
        //curr_prli.bcksctr = Eigen::MatrixXi::Zero(params.line_size, params.size_angle); // zeroing result backscatter

        const auto start = std::chrono::steady_clock::now();
        auto end = std::chrono::steady_clock::now();
        double elapsed_seconds; // timer for check how long this process go

        while (prli_ready < (ready_vec.size())) { // loop while backscatter not fill completely and conditions read

            try {
                auto first_byte = static_cast<int>(listen_first_byte()); // listening first byte of each parcel

                if (first_byte == 8 and prli_ready < (ready_vec.size())) { // if first byte is "0x8" listen PRLI data
                    prli_ready = listen_prli();
                    if (prli_ready == (ready_vec.size())) std::cout << "prli ready " << curr_prli.step << std::endl;

                } else if (first_byte == 1) { // if first byte is "0x1" listen PRLI data
                    cond_ready = listen_conditions();
                    if (cond_ready) std::cout << "cond ready " << curr_cond.lat << " " << curr_cond.lon << std::endl;

                } else {
                    std::stringstream buff;
                    buff << "Junk instead of first byte! Read byte >> " << first_byte << "!";
                    throw SeaVisionException(buff.str().c_str());
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
        return InputStructure{curr_cond, curr_prli};
    }

    InputProcessor::~InputProcessor() {
        close(socket_descriptor);
        WSACleanup();
    }

} // namespace