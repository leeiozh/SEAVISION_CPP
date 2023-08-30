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

    SocketParams params_prli{};     // internal socket parameters
    SocketParams params_navi{};     // internal socket parameters

    ReadParameters params_read;     // parameters of reading (number of lines through distance)
    InputPRLI curr_prli;            // current PRLI
    InputNavi curr_navi;            // current navigation conditions
    Eigen::VectorX<bool> ready_vec = Eigen::VectorX<bool>(AREA_AZIMUTH_PX); // vector of data readiness
    int double_counter = 0;         // counter for overloaded lines in backscatter

    int start_part, finish_part;    // number of start and finish reading parts

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

    /**
     * listening next parcel : navigation data + PRLI
     * @return
     */
    [[nodiscard]] InputStructure listen_message();

    /**
     * overloaded destructor
     */
    ~InputProcessor();
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
