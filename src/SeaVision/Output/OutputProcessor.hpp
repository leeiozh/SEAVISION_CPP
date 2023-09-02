//
// Created by leeiozh on 8/15/23.
//

#ifndef SEAVISION_OUTPUTPROCESSOR_HPP
#define SEAVISION_OUTPUTPROCESSOR_HPP

#ifdef WIN64
#include <winsock2.h>
#elif __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <string>
#include <utility>
#include <unistd.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

class OutputProcessor {

protected:
#ifdef WIN64
    WSADATA wsaData{};
#endif
    SocketParams params{};  // internal socket stuff

public:

    OutputProcessor(const std::string &ip, int port);

    void pass_message(const OutputStructure &output);

    ~OutputProcessor();
};

} // namespace

#endif //SEAVISION_OUTPUTPROCESSOR_HPP
