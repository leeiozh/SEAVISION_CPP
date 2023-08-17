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

public:
    InputProcessor(const std::string &ip, int port, const ReadParameters& params);

    InputStructure hear_one_message() const;

    ~InputProcessor();
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
