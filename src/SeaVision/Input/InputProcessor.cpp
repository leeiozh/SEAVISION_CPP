//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include "InputProcessor.hpp"

namespace SeaVision {

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
    client_addr_len = sizeof(client_address);
}

unsigned char InputProcessor::listen_first_byte() const {

    unsigned char first_byte;
    ssize_t bytesRead = recvfrom(socket_descriptor, (char *) &first_byte, sizeof(first_byte), 0,
                                 (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

    if (bytesRead == -1) {
        close(socket_descriptor);
        std::stringstream buff;
        buff << "Error when reading a first byte!";
        throw SeaVisionException(buff.str().c_str());
    }
    return first_byte;
}

InputConditions InputProcessor::listen_conditions() const {

    int32_t lat_lon[2];
    ssize_t bytesRead = recvfrom(socket_descriptor, (int32_t *) &lat_lon, sizeof(lat_lon), 0,
                                 (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

    if (bytesRead == -1) {
        close(socket_descriptor);
        std::stringstream buff;
        buff << "Error when reading a latitude or longitude!";
        throw SeaVisionException(buff.str().c_str());
    }

    double lat = static_cast<double>(lat_lon[0]) / 1000000.;
    double lon = static_cast<double>(lat_lon[1]) / 1000000.;

    uint16_t cdhs[4];
    bytesRead = recvfrom(socket_descriptor, (uint16_t *) &cdhs, sizeof(cdhs), 0,
                         (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

    if (bytesRead == -1) {
        close(socket_descriptor);
        std::stringstream buff;
        buff << "Error when reading a cog, sog, heading or speed!";
        throw SeaVisionException(buff.str().c_str());
    }

    double cog = static_cast<double>(cdhs[0]) / 100.;
    double sog = static_cast<double>(cdhs[1]) / 100.;
    double hdg = static_cast<double>(cdhs[2]) / 100.;
    double spd = static_cast<double>(cdhs[3]) / 100.;

    return InputConditions{cog, sog, lat, lon, spd, hdg};
}

InputPRLI InputProcessor::listen_prli() const {

    Eigen::MatrixX<bool> ready = Eigen::MatrixX<bool>::Zero(4096, 4);
    InputPRLI res;

    const auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    double elapsed_seconds;

    while (ready.sum() < ready.rows() * ready.cols()) {

        end = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration<double>(end - start).count();

        if (elapsed_seconds > MAX_TURN_PERIOD) {
            close(socket_descriptor);
            std::stringstream buff;
            buff << "Too much time for reading PRLI!";
            throw SeaVisionException(buff.str().c_str());
        }

        unsigned short num_step[2];
        ssize_t bytesRead = recvfrom(socket_descriptor, (unsigned short *) &num_step, sizeof(num_step), 0,
                                     (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

        if (bytesRead == -1) {
            close(socket_descriptor);
            std::stringstream buff;
            buff << "Error when reading a line number or step!";
            throw SeaVisionException(buff.str().c_str());
        }

        if (num_step[1] == 7) res.step = 1.875;
        else if (num_step[1] == 0) res.step = 3.75;
        else if (num_step[1] == 1) res.step = 7.5;
        else if (num_step[1] == 2) res.step = 15.0;
        else if (num_step[1] == 3) res.step = 30.0;
        else if (num_step[1] == 4) res.step = 60.0;
        else {
            close(socket_descriptor);
            std::stringstream buff;
            buff << "Error when convert a step!";
            throw SeaVisionException(buff.str().c_str());
        }

        unsigned char part_all[3]; // third byte reserved
        bytesRead = recvfrom(socket_descriptor, (unsigned short *) &part_all, sizeof(part_all), 0,
                             (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

        if (bytesRead == -1) {
            close(socket_descriptor);
            std::stringstream buff;
            buff << "Error when reading a number of line part!";
            throw SeaVisionException(buff.str().c_str());
        }

        unsigned char line[1024]; // third byte reserved
        bytesRead = recvfrom(socket_descriptor, (unsigned char *) &line, sizeof(line), 0,
                             (struct sockaddr *) &client_address, (socklen_t *) &client_addr_len);

        if (bytesRead == -1) {
            close(socket_descriptor);
            std::stringstream buff;
            buff << "Error when reading a PRLI!";
            throw SeaVisionException(buff.str().c_str());
        }

        if (params.line_start == 0 && params.line_end == 0) {
            for (int i = 1024 * part_all[0]; i < 1024 * (part_all[0] + 1); ++i) {
                res.bcksctr(i, num_step[0]) = line[i];
            }
        } else if (part_all[0] == 0) {
            for (int j = params.line_start; j < params.line_end; ++j) {
                res.bcksctr(j - params.line_start, num_step[0]) = line[j];
            }
        }
    }

    return res;
}

InputStructure InputProcessor::listen_message() const {

    bool prli_ready = false;
    bool cond_ready = false;
    InputConditions conds;
    InputPRLI prli;

    while (!prli_ready and !cond_ready) {

        auto first_byte = listen_first_byte();

        std::cout << "First byte " << std::hex << static_cast<int>(first_byte) << std::endl;

        if (first_byte == 0x0) {
            prli = listen_prli();
            prli_ready = true;
        }
        if (first_byte == 0x1) {
            conds = listen_conditions();
            cond_ready = true;
        }
    }
    return InputStructure{conds, prli};
}

InputProcessor::~InputProcessor() {
    close(socket_descriptor);
}

} // namespace