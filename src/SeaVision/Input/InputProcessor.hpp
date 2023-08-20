//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTPROCESSOR_HPP
#define SEAVISION_INPUTPROCESSOR_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <chrono>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

class InputProcessor {

protected:
    std::string ip;
    int port;
    int socket_descriptor;
    struct sockaddr_in server_address{};
    ReadParameters params;
    InputPRLI curr_prli;
    Eigen::MatrixX<bool> ready;

public:
    InputProcessor(const std::string &ip, int port, const ReadParameters& params);

    [[nodiscard]] unsigned char listen_first_byte() const;

    [[nodiscard]] InputStructure listen_message();

    [[nodiscard]] InputConditions listen_conditions() const;

    [[nodiscard]] bool listen_prli();

    ~InputProcessor();
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
