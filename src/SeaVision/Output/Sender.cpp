//
// Created by leeiozh on 8/16/23.
//

#include "Sender.hpp"

namespace SeaVision {

Sender::Sender(const std::string &ip, int port) :
        ip(ip), port(port) {

    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor == -1) {
        std::stringstream buff;
        buff << "Error when creating a socket!";
        throw SeaVisionException(buff.str().c_str());
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);
}

void Sender::pass_one_file(const std::string &path, const std::string &file_name) {

    std::ifstream file((path + file_name).c_str(), std::ios::in | std::ios::binary);

    if (file.is_open()) {

        double meta[8];
        file.read((char *) &meta, sizeof(meta));
        ssize_t bytesSent = sendto(socket_descriptor, meta, sizeof(meta), 0,
                                   (struct sockaddr *) &server_address, sizeof(server_address));

        if (bytesSent == -1) {
            std::stringstream buff;
            buff << "Error when sending a data!";
            throw SeaVisionException(buff.str().c_str());
        }


        for (int i = 0; i < 4096; ++i) {

            uint8_t curr_line[4096];
            file.read((char *) &curr_line, sizeof(curr_line));

            bytesSent = sendto(socket_descriptor, curr_line, sizeof(curr_line), 0,
                               (struct sockaddr *) &server_address, sizeof(server_address));

            uint8_t junk[8];
            file.read((char *) &junk, sizeof(junk));
            bytesSent = sendto(socket_descriptor, junk, sizeof(junk), 0,
                               (struct sockaddr *) &server_address, sizeof(server_address));
        }

        file.close();

    } else {
        std::stringstream buff;
        buff << "Error when reading a file!";
        throw SeaVisionException(buff.str().c_str());
    }
}

void Sender::pass_queue_files(const std::string &path, int num) {

    std::vector<std::filesystem::path> filenames;
    for (const auto &entry: std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file() && std::filesystem::file_size(entry) > 1) {
            filenames.push_back(entry.path().filename());
        }
    }

    std::sort(filenames.begin(), filenames.end(),
              [](const auto &lhs, const auto &rhs) {
                  return lhs.string() < rhs.string();
              });

    if (num > static_cast<int>(filenames.size())) {
        std::stringstream buff;
        buff << "Not enough file in " << path << ". You request " << num << " files, but there is only " <<
             static_cast<int>(filenames.size());
        throw SeaVisionException(buff.str().c_str());
    }

    for (int i = 0; i < num; ++i) {
        pass_one_file(path, filenames[i]);
    }
}

Sender::~Sender() {
    close(socket_descriptor);
}

} // namespace