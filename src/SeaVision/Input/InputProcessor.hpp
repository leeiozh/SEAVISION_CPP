//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTPROCESSOR_HPP
#define SEAVISION_INPUTPROCESSOR_HPP

#include "../Structures.hpp"
#include <string>

namespace SeaVision {
class InputProcessor {
protected:
    std::string input_ip;
    int input_port;

public:
    InputProcessor(const std::string &ip, const int &port) : input_ip(ip), input_port(port) {};

    InputStructure hear_one_message(const int &buffer);
};

} // namespace

#endif //SEAVISION_INPUTPROCESSOR_HPP
