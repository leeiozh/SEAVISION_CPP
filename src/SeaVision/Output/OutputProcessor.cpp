//
// Created by leeiozh on 8/15/23.
//

#include "OutputProcessor.hpp"

namespace SeaVision {

OutputProcessor::OutputProcessor(const std::string &ip, const int port) {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw SeaVisionException("Failed to initialize Winsock!");
    }

    socket_params.socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_params.socket_descriptor == INVALID_SOCKET) {
        WSACleanup();
        throw SeaVisionException("Failed to create socket!");
    }

    socket_params.server_address.sin_family = AF_INET;
    socket_params.server_address.sin_port = htons(port);
    socket_params.server_address.sin_addr.s_addr = inet_addr(ip.c_str());
}

void OutputProcessor::pass_message(const OutputStructure &output) {

    unsigned char data[25 + NUM_AREA * 2];
    data[0] = 0x5;

    for (int i = 0; i < NUM_SYSTEMS; ++i) {
        data[1 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.swh[i] * 100)) & 0xFF);
        data[2 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.swh[i] * 100) >> 8) & 0xFF);

        data[3 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.dir[i] * 100)) & 0xFF);
        data[4 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.dir[i] * 100) >> 8) & 0xFF);

        data[5 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.per[i] * 100)) & 0xFF);
        data[6 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.per[i] * 100) >> 8) & 0xFF);

        data[7 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.len[i] * 100)) & 0xFF);
        data[8 + 8 * i] = static_cast<unsigned char>((static_cast<uint16_t>(output.len[i] * 100) >> 8) & 0xFF);
    }

    for (int i = 0; i < NUM_AREA; ++i) {
        data[25 + i * 2] = static_cast<unsigned char>((static_cast<uint16_t>(output.rose[i] * 100)) & 0xFF);
        data[25 + i * 2 + 1] = static_cast<unsigned char>((static_cast<uint16_t>(output.rose[i] * 100) >> 8) & 0xFF);
    }

    ssize_t bytesSent = sendto(socket_params.socket_descriptor, reinterpret_cast<const char *>(&data), sizeof(data), 0,
                               (struct sockaddr *) &socket_params.server_address, sizeof(socket_params.server_address));
    if (bytesSent == -1) throw SeaVisionException("Error when sending a result!");

}

OutputProcessor::~OutputProcessor() {
    close(socket_params.socket_descriptor);
    WSACleanup();
}

} // namespace