//
// Created by leeiozh on 8/15/23.
//

#ifndef SEAVISION_OUTPUTPROCESSOR_HPP
#define SEAVISION_OUTPUTPROCESSOR_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <unistd.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

class OutputProcessor {

protected:
    std::string ip;
    int port;
    int socket_descriptor;
    struct sockaddr_in server_address{};

public:

    OutputProcessor(const std::string &ip, int port);

    void pass_one_message(const OutputStructure &output);

    ~OutputProcessor();
};

} // namespace

#endif //SEAVISION_OUTPUTPROCESSOR_HPP
