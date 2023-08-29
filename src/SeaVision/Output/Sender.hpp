//
// Created by leeiozh on 8/16/23.
//

#ifndef SEAVISION_SENDER_HPP
#define SEAVISION_SENDER_HPP

#include <winsock2.h>
#include <string>
#include <utility>
#include <unistd.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"
#include "SeaVision/Input/FileReader.hpp"

namespace SeaVision {

class Sender {

protected:
    WSADATA wsaData{};
    SOCKET descriptor;
    sockaddr_in address;
    std::string ip;
    int port;

    std::shared_ptr<FileReader> file_reader;

public:

    Sender(const std::string &ip, int port, const std::shared_ptr<FileReader>& file_reader);

    void pass_queue_files(const std::string &path, int num);

    void pass_one_file(const std::string &file_name);

    void pass_cond(const InputNavi &cond);

    void pass_prli(const InputPRLI &prli);

    ~Sender();

};

}

#endif //SEAVISION_SENDER_HPP
