//
// Created by leeiozh on 8/15/23.
//

#include "OutputProcessor.hpp"

namespace SeaVision {

OutputProcessor::OutputProcessor(const std::string &ip, const int port) : ip(ip), port(port) {

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

void OutputProcessor::pass_message(const OutputStructure &output) {

    unsigned char first_byte = 0x5;

    ssize_t bytesSent = sendto(descriptor, reinterpret_cast<const char *>(&first_byte), sizeof(first_byte), 0,
                               (struct sockaddr *) &address, sizeof(address));

    if (bytesSent == -1) throw SeaVisionException("Error when sending a first byte of result!");


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

    bytesSent = sendto(descriptor, reinterpret_cast<const char *>(data), sizeof(data), 0,
                       (struct sockaddr *) &address, sizeof(address));

    if (bytesSent == -1) throw SeaVisionException("Error when sending a wave parameters!");


    uint8_t rose[NUM_AREA];
    for (int i = 0; i < NUM_AREA; ++i) {
        rose[i] = static_cast<uint8_t>(output.rose[i] * 100);
    }

    bytesSent = sendto(descriptor, reinterpret_cast<const char *>(rose), sizeof(rose), 0,
                       (struct sockaddr *) &address, sizeof(address));

    if (bytesSent == -1) throw SeaVisionException("Error when sending a wind rose!");

}

OutputProcessor::~OutputProcessor() {
    close(descriptor);
    WSACleanup();
}

} // namespace