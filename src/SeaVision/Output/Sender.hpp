//
// Created by leeiozh on 8/16/23.
//

#ifndef SEAVISION_SENDER_HPP
#define SEAVISION_SENDER_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <unistd.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"
#include "SeaVision/Input/FileReader.hpp"

namespace SeaVision {

class Sender {

protected:
    std::string ip;
    int port;
    int socket_descriptor;
    struct sockaddr_in server_address{};

public:

    Sender(const std::string &ip, int port);

    void pass_one_file(const std::string &path, const std::string &file_name);

    void pass_queue_files(const std::string &path, int num);

    ~Sender();

};

}

#endif //SEAVISION_SENDER_HPP
