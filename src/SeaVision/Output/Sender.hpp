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

    std::shared_ptr<FileReader> file_reader;

public:

    Sender(const std::string &ip, int port, const std::shared_ptr<FileReader>& file_reader);

    void pass_queue_files(const std::string &path, int num);

    void pass_one_file(const std::string &file_name);

    void pass_cond(const InputConditions &cond);

    void pass_prli(const InputPRLI &prli);

    ~Sender();

};

}

#endif //SEAVISION_SENDER_HPP
