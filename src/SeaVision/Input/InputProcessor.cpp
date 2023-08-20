//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include <fstream>
#include "InputProcessor.hpp"

namespace SeaVision {

std::ofstream file("/home/leeiozh/ocean/SEAVISION_CPP/tests/text.txt");

InputProcessor::InputProcessor(const std::string &ip, int port, const ReadParameters &params) : ip(ip), port(port),
                                                                                                params(params) {
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

    ready.resize(4096, 4);
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

InputConditions InputProcessor::listen_conditions() const {

    int32_t lat_lon[2];
    ssize_t bytesRead = recv(socket_descriptor, (int32_t *) lat_lon, sizeof(lat_lon), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a latitude or longitude!";
        throw SeaVisionException(buff.str().c_str());
    }

    double lat = static_cast<double>(lat_lon[0]) / 1000000.;
    double lon = static_cast<double>(lat_lon[1]) / 1000000.;

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

    file << "cshs " << cog << " " << sog << " " << hdg << " " << spd << std::endl;

    return InputConditions{cog, sog, lat, lon, spd, hdg};
}

bool InputProcessor::listen_prli() {

    unsigned short num_step[2];
    ssize_t bytesRead = recv(socket_descriptor, (unsigned short *) &num_step, sizeof(num_step), 0);

    int num = static_cast<int>(num_step[0]);

    curr_prli.step = static_cast<double>(num_step[1]) / 1000.;

    file << "received " << std::dec << num << " " << curr_prli.step << " ";

    if (bytesRead == -1 or num >= 4096 or num < 0) {
        std::stringstream buff;
        buff << "Error when reading a line number or step!";
        throw SeaVisionException(buff.str().c_str());
    }

    unsigned char part_all[3]; // third byte reserved
    bytesRead = recv(socket_descriptor, (unsigned char *) &part_all, sizeof(part_all), 0);
    int part = static_cast<int>(part_all[0]);

    if (bytesRead == -1 or part < 0 or part > 4) {
        std::stringstream buff;
        buff << "Error when reading a number of line part!";
        throw SeaVisionException(buff.str().c_str());
    }

    file << part << " " << static_cast<int>(part_all[1]) << " " <<
         static_cast<int>(part_all[2]) << std::endl;

    unsigned char line[1024];
    bytesRead = recv(socket_descriptor, (unsigned char *) &line, sizeof(line), 0);

    if (bytesRead == -1) {
        std::stringstream buff;
        buff << "Error when reading a PRLI!";
        throw SeaVisionException(buff.str().c_str());
    }

    if (params.line_start == 0 && params.line_end == 0) {
        for (int i = 1024 * part; i < 1024 * (part + 1); ++i) {
            curr_prli.bcksctr(i, num) = static_cast<int>(line[i - 1024 * part]);
            ready(num, part) = true;
        }
    } else if (part == 0 and params.line_end < 1024) {
        for (int j = params.line_start; j < params.line_end; ++j) {
            curr_prli.bcksctr(j - params.line_start, num) = static_cast<int>(line[j]);
            ready(num, part) = true;
        }
    }
    std::cout << ready.sum() / static_cast<double >(ready.rows() * ready.cols()) << std::endl;
    return (ready.sum() >= 0.95 * static_cast<double >(ready.rows() * ready.cols()));
}

InputStructure InputProcessor::listen_message() {

    bool prli_ready = false;
    bool cond_ready = false;
    InputConditions conds;
    ready = Eigen::MatrixX<bool>::Zero(4096, 4);

    const auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    double elapsed_seconds;

    while (!prli_ready or !cond_ready) {

        auto first_byte = listen_first_byte();

        file << "First byte " << std::hex << static_cast<int>(first_byte) << " ";

        if (first_byte == 0x8) {
            prli_ready = listen_prli();
            if (prli_ready) std::cout << "prli ready" << std::endl;
        }
        if (first_byte == 0x1 and !cond_ready) {
            conds = listen_conditions();
            cond_ready = true;
            std::cout << "cond ready" << std::endl;
        } else {
            std::stringstream buff;
            buff << "Junk instead of first byte!";
            file << std::endl;
            throw SeaVisionException(buff.str().c_str());
        }

        end = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration<double>(end - start).count();

        if (elapsed_seconds > MAX_TURN_PERIOD) {
            std::stringstream buff;
            buff << "Too much time for reading PRLI!";
            throw SeaVisionException(buff.str().c_str());
        }
    }
    return InputStructure{conds, curr_prli};
}

InputProcessor::~InputProcessor() {
    close(socket_descriptor);
}

} // namespace