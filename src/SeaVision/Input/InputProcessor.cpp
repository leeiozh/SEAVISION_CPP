//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include <fstream>
#include "InputProcessor.hpp"

namespace SeaVision {

std::ofstream file("/home/leeiozh/ocean/seavisionCPP/SeaVision/tests/log.txt");

InputProcessor::InputProcessor(const std::string &ip, int port, const ReadParameters &params) : ip(ip), port(port),
                                                                                                params(params) {
    // creating a socket
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor == -1) {
        std::stringstream buff;
        buff << "Error when creating a socket!";
        throw SeaVisionException(buff.str().c_str());
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

    // binding a socket
    if (bind(socket_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        std::stringstream buff;
        buff << "Error when binding a socket!";
        throw SeaVisionException(buff.str().c_str());
    }

    ready_vec.resize(params.size_angle); // resizing vector of readiness
    curr_prli.bcksctr.resize(params.line_size, params.size_angle);

    start_part = params.line_start / 1024;
    finish_part = params.line_end / 1024;
}

unsigned char InputProcessor::listen_first_byte() const {

    unsigned char first_byte;
    ssize_t bytesRead = recv(socket_descriptor, &first_byte, sizeof(first_byte), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a first byte!";
        throw SeaVisionException(buff.str().c_str());
    }
    return first_byte;
}

bool InputProcessor::listen_conditions() {

    int32_t lat_lon[2];
    ssize_t bytesRead = recv(socket_descriptor, (int32_t *) lat_lon, sizeof(lat_lon), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a latitude and longitude!";
        throw SeaVisionException(buff.str().c_str());
    }

    double lat = static_cast<double>(lat_lon[0]) / 1000000.;
    double lon = static_cast<double>(lat_lon[1]) / 1000000.;

    // adequacy check
    if (lat < -180. or lat > 180. or lon < -90. or lon > 90.) return false;

    file << "lat_lon " << lat << " " << lon << std::endl;

    uint16_t cshs[4];
    bytesRead = recv(socket_descriptor, (uint16_t *) cshs, sizeof(cshs), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a cog, sog, heading or speed!";
        throw SeaVisionException(buff.str().c_str());
    }

    double cog = static_cast<double>(cshs[0]) / 100.;
    double sog = static_cast<double>(cshs[1]) / 100.;
    double hdg = static_cast<double>(cshs[2]) / 100.;
    double spd = static_cast<double>(cshs[3]) / 100.;

    // adequacy check
    if (cog < 0. or cog > 360. or sog < 0. or sog > 1000. or hdg < 0. or hdg > 360. or spd < 0. or spd > 1000.)
        return false;

    curr_cond = InputConditions{cog, sog, lat, lon, spd, hdg};

    file << "cshs " << curr_cond.cog << " " << curr_cond.sog << " " << curr_cond.hdg << " " << curr_cond.spd
         << std::endl;

    return true; // only if the result is adequate
}

int InputProcessor::listen_prli() {

    unsigned short num_step[2];
    ssize_t bytesRead = recv(socket_descriptor, (unsigned short *) &num_step, sizeof(num_step), 0);

    int num = static_cast<int>(num_step[0]); // number of received line (from 0 to 4096)

    curr_prli.step = static_cast<double>(num_step[1]) / 1000.; // distance in meters between neighbour cells

    if (bytesRead == -1 or num >= 4096 or num < 0 or curr_prli.step < 1.) { // adequacy check
        std::stringstream buff;
        buff << "Error when reading a line number or step!";
        throw SeaVisionException(buff.str().c_str());
    }

    unsigned char part_all[3]; // second byte is 4, third byte reserved
    bytesRead = recv(socket_descriptor, (unsigned char *) &part_all, sizeof(part_all), 0);

    int part = static_cast<int>(part_all[0]); // number of line part (from 0 to 4)

    if (bytesRead == -1 or part < 0 or part > 4) { // adequacy check
        std::stringstream buff;
        buff << "Error when reading a number of line part! Read part >> " << part << "!";
        throw SeaVisionException(buff.str().c_str());
    }

    //file << part << " " << static_cast<int>(part_all[1]) << " " <<
    //     static_cast<int>(part_all[2]) << std::endl;

    unsigned char line[1024];
    bytesRead = recv(socket_descriptor, (unsigned char *) &line, sizeof(line), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a PRLI!";
        throw SeaVisionException(buff.str().c_str());
    }

    if (part >= start_part and part < finish_part) {
        for (int i = 0; i < 1024; ++i) {
            curr_prli.bcksctr(i - params.line_start + part * i, num) = static_cast<int>(line[i]);
        }
    }

    if (part == 0) ready_vec(num) = true; // think that line with this number is ready

    /*std::cout << num << " " << part << " sum " << ready_matrix.cast<int>().sum() << " "
              << static_cast<double >(ready_matrix.cast<int>().sum()) /
                 static_cast<double >(ready_matrix.rows() * ready_matrix.cols()) << std::endl;*/

    return ready_vec.cast<int>().sum();
}

InputStructure InputProcessor::listen_message() {

    int prli_ready = 0.; // number of ready lines
    bool cond_ready = false; // flag if conditions are ready
    ready_vec = Eigen::VectorX<bool>::Zero(params.size_angle); // zeroing vector of readiness
    curr_prli.bcksctr = Eigen::MatrixXi::Zero(params.line_size, params.size_angle); // zeroing result backscatter

    const auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    double elapsed_seconds; // timer for check how long this process go

    while (prli_ready < (ready_vec.size()) or
           !cond_ready) { // loop while backscatter not fill completely and conditions read

        try {
            auto first_byte = static_cast<int>(listen_first_byte()); // listening first byte of each parcel

            if (first_byte == 8 and prli_ready < (ready_vec.size())) { // if first byte is "0x8" listen PRLI data
                prli_ready = listen_prli();
                if (prli_ready == (ready_vec.size())) std::cout << "prli ready" << std::endl;
            } else if (first_byte == 1 and !cond_ready) { // if first byte is "0x1" listen PRLI data
                cond_ready = listen_conditions();
                if (cond_ready) std::cout << "cond ready" << std::endl;
            } else {
                std::stringstream buff;
                buff << "Junk instead of first byte! Read byte >> " << first_byte << "!";
                throw SeaVisionException(buff.str().c_str());
            }

        } catch (const SeaVisionException &exception) {
            //std::cerr << "Local read error: " << exception.what() << std::endl;
            continue;
        }

        end = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration<double>(end - start).count();

        if (elapsed_seconds > MAX_TURN_PERIOD) { // if process go more time than expected
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
}

} // namespace