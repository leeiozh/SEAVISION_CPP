//
// Created by leeiozh on 13.04.23.
//

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
}

InputStructure InputProcessor::hear_one_message() const {
    struct sockaddr_in client_address{};
    socklen_t client_addr_len = sizeof(client_address);

    double meta[9];
    ssize_t bytesRead = recvfrom(socket_descriptor, (char *) &meta, sizeof(meta), 0,
                                 (struct sockaddr *) &client_address, &client_addr_len);

    if (bytesRead == -1) {
        close(socket_descriptor);
        std::stringstream buff;
        buff << "Error when reading a data!";
        throw SeaVisionException(buff.str().c_str());
    }

    if (meta[8] != STEP) {
        close(socket_descriptor);
        std::stringstream buff;
        buff << "Not SP1!";
        throw SeaVisionException(buff.str().c_str());
    }

    InputStructure res{meta[6], meta[7], meta[3], meta[4], meta[5], meta[8], 464, 4096};

    // reading throw one azimuth
    for (int i = 0; i < res.size_az; ++i) {

        // read full line throw current azimuth
        uint8_t curr_line[res.size_az];
        bytesRead = recvfrom(socket_descriptor, (char *) &curr_line, sizeof(curr_line), 0,
                             (struct sockaddr *) &client_address, &client_addr_len);

        // read some metadata which we don't used
        uint8_t junk[8];
        bytesRead = recvfrom(socket_descriptor, (char *) &junk, sizeof(junk), 0,
                             (struct sockaddr *) &client_address, &client_addr_len);

        // record necessary data

        if (params.line_start == 0 && params.line_end == 0) {
            res.size_dist = res.size_az;
            for (int j = 0; j < res.size_dist; ++j) {
                res.bcksctr(j, i) = curr_line[j];
            }
        } else {
            for (int j = params.line_start; j < params.line_end; ++j) {
                res.bcksctr(j - params.line_start, i) = curr_line[j];
            }
        }
    }
    return res;
}

InputProcessor::~InputProcessor() {
    close(socket_descriptor);
}

} // namespace