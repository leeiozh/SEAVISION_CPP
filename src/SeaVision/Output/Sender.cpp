//
// Created by leeiozh on 8/16/23.
//
#include <thread>
#include <chrono>
#include "Sender.hpp"

namespace SeaVision {

Sender::Sender(const std::string &ip, int port, const std::shared_ptr<FileReader>& file_reader) :
        ip(ip), port(port), file_reader(file_reader) {

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

void Sender::pass_cond(const SeaVision::InputConditions &cond) {

    unsigned char first_byte[1];
    first_byte[0] = 0x1;
    sendto(socket_descriptor, first_byte, sizeof(first_byte), 0, (struct sockaddr *) &server_address,
           sizeof(server_address));

    int32_t lat_lon[2];
    lat_lon[0] = static_cast<int32_t>(1000000 * cond.lat);
    lat_lon[1] = static_cast<int32_t>(1000000 * cond.lon);

    sendto(socket_descriptor, lat_lon, sizeof(lat_lon), 0, (struct sockaddr *) &server_address,
           sizeof(server_address));

    uint16_t cdhs[4];
    cdhs[0] = static_cast<uint16_t>(cond.cog * 100.);
    cdhs[1] = static_cast<uint16_t>(cond.sog * 100.);
    cdhs[2] = static_cast<uint16_t>(cond.hdg * 100.);
    cdhs[3] = static_cast<uint16_t>(cond.spd * 100.);

    sendto(socket_descriptor, cdhs, sizeof(cdhs), 0, (struct sockaddr *) &server_address,
           sizeof(server_address));
}

void Sender::pass_prli(const SeaVision::InputPRLI &prli) {
    unsigned char first_byte[1];
    first_byte[0] = 0x8;
    sendto(socket_descriptor, first_byte, sizeof(first_byte), 0, (struct sockaddr *) &server_address,
           sizeof(server_address));

    unsigned short num_step[2];

    for (int i = 0; i < 4096; ++i) {
        num_step[0] = i;
        num_step[1] = 7;
        sendto(socket_descriptor, num_step, sizeof(num_step), 0, (struct sockaddr *) &server_address,
               sizeof(server_address));

        for (int j = 0; j < 4; ++j) {
            unsigned char part_all[3];
            part_all[0] = j;
            part_all[1] = 4;
            part_all[2] = 0;

            sendto(socket_descriptor, part_all, sizeof(part_all), 0, (struct sockaddr *) &server_address,
                   sizeof(server_address));

            unsigned char line[1024];
            for (int k = 0; k < 1024; ++k){
                line[k] = prli.bcksctr(j * 1024 + k, j);
            }
            sendto(socket_descriptor, line, sizeof(line), 0, (struct sockaddr *) &server_address,
                   sizeof(server_address));
        }
    }
}

void Sender::pass_one_file(const std::string &file_name) {

    InputStructure inp = file_reader->read_one_file(file_name);
    pass_cond(inp.cond);
    std::cout << file_name << " conditions send" << std::endl;
    pass_prli(inp.prli);
    std::cout << file_name << " prli send" << std::endl;
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
        pass_one_file(filenames[i]);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << i << "th file send" << std::endl;
    }
}

Sender::~Sender() {
    close(socket_descriptor);
}

} // namespace