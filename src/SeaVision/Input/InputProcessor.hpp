//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTPROCESSOR_HPP
#define SEAVISION_INPUTPROCESSOR_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <utility>
#include <unistd.h>
#include <chrono>
#include "SeaVision/Structures.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

class InputProcessor {
    /**
     * class of receiving data via UDP connect and process it for next work
     */

protected:
    std::string ip; // IP of current machine
    int port; // port for receiving data
    int socket_descriptor; // internal stuff
    struct sockaddr_in server_address{}; // internal stuff
    ReadParameters params; // parameters of reading (number of lines through distance)
    InputPRLI curr_prli; // current PRLI for filling
    InputConditions curr_cond; // current conditions for filling
    Eigen::VectorX<bool> ready_vec; // vector of readiness data
    int double_counter;

    int start_part, finish_part;

public:
    /**
     * constructor of input processor
     * @param ip IP of current machine
     * @param port port for receiving data
     * @param params parameters of reading (number of lines through distance)
     */
    InputProcessor(const std::string &ip, int port, const ReadParameters& params);

    /**
     * listening first byte of each parcel
     * @return first byte
     */
    [[nodiscard]] unsigned char listen_first_byte() const;

    /**
     * listening next parcel and filling result InputStructure
     * @return result InputStructure
     */
    [[nodiscard]] InputStructure listen_message();

    /**
     * listening conditions
     * @return true if conditions listened successfully
     */
    [[nodiscard]] bool listen_conditions();

    /**
     * listening PRLI
     * @return number of ready lines (calculates using ready_vec)
     */
    [[nodiscard]] int listen_prli();

    /**
     * destructor
     */
    ~InputProcessor();
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
