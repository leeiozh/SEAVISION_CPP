//
// Created by leeiozh on 8/15/23.
//

#include <iostream>
#include "OutputProcessor.hpp"

namespace SeaVision {

OutputProcessor::OutputProcessor(const std::string &ip, const int port) : ip(ip), port(port) {
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

void OutputProcessor::pass_message(const OutputStructure &output) {

    unsigned char first_byte = 0x5;

    ssize_t bytesSent = sendto(socket_descriptor, &first_byte, sizeof(first_byte), 0,
                               (struct sockaddr *) &server_address, sizeof(server_address));

    if (bytesSent == -1) {
        std::stringstream buff;
        buff << "Error when sending a data!";
        throw SeaVisionException(buff.str().c_str());
    }

    uint16_t data[12];

    data[0] = static_cast<uint16_t>(output.swh[0] * 100);
    data[1] = static_cast<uint16_t>(output.dir[0] * 100);
    data[2] = static_cast<uint16_t>(output.per[0] * 100);
    data[3] = static_cast<uint16_t>(output.len[0] * 100);
    data[4] = static_cast<uint16_t>(output.swh[1] * 100);
    data[5] = static_cast<uint16_t>(output.dir[1] * 100);
    data[6] = static_cast<uint16_t>(output.per[1] * 100);
    data[7] = static_cast<uint16_t>(output.len[1] * 100);
    data[8] = static_cast<uint16_t>(output.swh[2] * 100);
    data[9] = static_cast<uint16_t>(output.dir[2] * 100);
    data[10] = static_cast<uint16_t>(output.per[2] * 100);
    data[11] = static_cast<uint16_t>(output.len[2] * 100);

    bytesSent = sendto(socket_descriptor, data, sizeof(data), 0,
                               (struct sockaddr *) &server_address, sizeof(server_address));

    if (bytesSent == -1) {
        std::stringstream buff;
        buff << "Error when sending a data!";
        throw SeaVisionException(buff.str().c_str());
    }

    uint8_t rose[NUM_AREA];
    for (int i = 0; i < NUM_AREA; ++i) {
        rose[i] = static_cast<uint8_t>(output.rose[i] * 100);
    }

    bytesSent = sendto(socket_descriptor, rose, sizeof(rose), 0,
                               (struct sockaddr *) &server_address, sizeof(server_address));

    if (bytesSent == -1) {
        std::stringstream buff;
        buff << "Error when sending a data!";
        throw SeaVisionException(buff.str().c_str());
    }
}

OutputProcessor::~OutputProcessor() {
    close(socket_descriptor);
}

} // namespace