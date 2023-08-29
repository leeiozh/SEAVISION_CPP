//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTPROCESSOR_HPP
#define SEAVISION_INPUTPROCESSOR_HPP

#include <winsock2.h>
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
    WSADATA wsaData{};

    SocketParams params_prli{};
    SocketParams params_navi{};

    ReadParameters params_read;     // parameters of reading (number of lines through distance)
    InputPRLI curr_prli;            // current PRLI for filling
    InputNavi curr_navi;      // current conditions for filling
    Eigen::VectorX<bool> ready_vec; // vector of readiness data
    int double_counter;

    int start_part, finish_part;

public:
    /**
     * constructor of input processor
     * @param port port for receiving data
     * @param params parameters of reading (number of lines through distance)
     */
    InputProcessor(int prli_port, int navi_port, const ReadParameters &params);

    /**
     * listening PRLI
     * @return number of ready lines (calculates using ready_vec)
     */
    [[nodiscard]] int listen_prli();

    /**
     * listening navigation conditions
     * @return true if conditions listened successfully
     */
    [[nodiscard]] bool listen_navi();

    [[nodiscard]] InputStructure listen_message();

    /**
     * destructor
     */
    ~InputProcessor();
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
