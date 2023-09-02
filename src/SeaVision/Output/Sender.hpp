//
// Created by leeiozh on 8/16/23.
//

#ifndef SEAVISION_SENDER_HPP
#define SEAVISION_SENDER_HPP

#ifdef WIN32
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
#include "SeaVision/Input/FileReader.hpp"

namespace SeaVision {

class Sender {

protected:
#ifdef WIN64
    WSADATA wsaData{};
#endif

    SocketParams params{};

    std::unique_ptr<FileReader> file_reader;

public:

    Sender(const std::string &ip, int port, std::unique_ptr<FileReader> file_reader);

    void pass_queue_files(const std::string &path, int num);

    void pass_one_file(const std::string &file_name);

    void pass_cond(const InputNavi &cond);

    void pass_prli(const InputPRLI &prli);

    ~Sender();

};

}

#endif //SEAVISION_SENDER_HPP
