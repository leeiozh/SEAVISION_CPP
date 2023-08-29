//
// Created by leeiozh on 8/16/23.
//
#include <thread>
#include <chrono>
#include "Sender.hpp"

namespace SeaVision {

    Sender::Sender(const std::string &ip, int port, const std::shared_ptr<FileReader> &file_reader) :
            ip(ip), port(port), file_reader(file_reader) {

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw SeaVisionException("Failed to initialize Winsock!");
        }

        descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (descriptor == INVALID_SOCKET) {
            WSACleanup();
            throw SeaVisionException("Failed to create socket!");
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    void Sender::pass_cond(const SeaVision::InputNavi &cond) {

        unsigned char first_byte = 0x1;
        sendto(descriptor, reinterpret_cast<const char *>(&first_byte), sizeof(first_byte), 0,
               (struct sockaddr *) &address, sizeof(address));

        int32_t lat_lon[2];
        lat_lon[0] = static_cast<int32_t>(1000000 * cond.lat);
        lat_lon[1] = static_cast<int32_t>(1000000 * cond.lon);

        sendto(descriptor, reinterpret_cast<const char *>(lat_lon), sizeof(lat_lon), 0, (struct sockaddr *) &address,
               sizeof(address));

        std::cout << "send lat+lon " << cond.lat << " " << cond.lon << std::endl;

        uint16_t cshs[4];
        cshs[0] = static_cast<uint16_t>(cond.cog * 100.);
        cshs[1] = static_cast<uint16_t>(cond.sog * 100.);
        cshs[2] = static_cast<uint16_t>(cond.hdg * 100.);
        cshs[3] = static_cast<uint16_t>(cond.spd * 100.);

        sendto(descriptor, reinterpret_cast<const char *>(cshs), sizeof(cshs), 0, (struct sockaddr *) &address,
               sizeof(address));

        std::cout << "send cdhs " << cond.cog << " " << cond.sog << " " << cond.hdg << " " << cond.spd << std::endl;
    }

    void Sender::pass_prli(const SeaVision::InputPRLI &prli) {

        unsigned char first_byte = 0x8;

        unsigned short num_step[2];

        for (int i = 0; i < 4096; ++i) {
            num_step[0] = static_cast<unsigned short>(i);
            num_step[1] = static_cast<unsigned short>(1875);

            for (int j = 0; j < 4; ++j) {
                unsigned char part_all[3];
                part_all[0] = static_cast<unsigned char>(j);
                part_all[1] = static_cast<unsigned char>(4);
                part_all[2] = static_cast<unsigned char>(0);

                sendto(descriptor, reinterpret_cast<const char *>(&first_byte), sizeof(first_byte), 0,
                       (struct sockaddr *) &address, sizeof(address));

                //std::cout << "prli byte send" << std::endl;
                //std::this_thread::sleep_for(std::chrono::seconds(1));

                sendto(descriptor, reinterpret_cast<const char *>(num_step), sizeof(num_step), 0,
                       (struct sockaddr *) &address, sizeof(address));

                //std::cout << "prli num_step send " << i << std::endl;
                //std::this_thread::sleep_for(std::chrono::seconds(1));

                sendto(descriptor, reinterpret_cast<const char *>(part_all), sizeof(part_all), 0,
                       (struct sockaddr *) &address, sizeof(address));

                //std::cout << "prli part_all send " << j << std::endl;
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));

                unsigned char line[1024];
                for (int k = 0; k < 1024; ++k) {
                    line[k] = static_cast<unsigned char>(prli.bcksctr(j * 1024 + k, i));
                }
                sendto(descriptor, reinterpret_cast<const char *>(line), sizeof(line), 0, (struct sockaddr *) &address,
                       sizeof(address));

                //std::cout << "prli " << i << " " << j << " line send" << std::endl;
                std::this_thread::sleep_for(std::chrono::microseconds(150));
            }
            //std::cout << "prli " << i << " line send" << std::endl;
        }
    }

    void Sender::pass_one_file(const std::string &file_name) {

        InputStructure inp = file_reader->read_one_file(file_name);
        pass_cond(inp.navi);
        std::cout << file_name << " conditions send" << std::endl;
        pass_prli(inp.prli);
        std::cout << file_name << " prli send" << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void Sender::pass_queue_files(const std::string &path, int num) {

        std::vector<std::filesystem::path> filenames;
        for (const auto &entry: std::filesystem::directory_iterator{path}) {
            if (entry.is_regular_file() && std::filesystem::file_size(entry) > 1) {
                filenames.push_back(entry.path().filename());
            }
        }

        std::sort(filenames.begin(), filenames.end(),
                  [](const auto &lhs, const auto &rhs) { return lhs.string() < rhs.string(); });

        if (num > static_cast<int>(filenames.size())) {
            std::stringstream buff;
            buff << "Not enough file in " << path << ". You request " << num << " files, but there is only " <<
                 static_cast<int>(filenames.size());
            throw SeaVisionException(buff.str().c_str());
        }

        for (int i = 0; i < num; ++i) {
            std::cout << i << " ";
            pass_one_file(filenames[i].string());
        }
    }

    Sender::~Sender() {
        close(descriptor);
        WSACleanup();
    }

} // namespace