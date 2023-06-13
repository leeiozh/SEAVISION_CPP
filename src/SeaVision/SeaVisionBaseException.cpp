//
// Created by leeiozh on 02.08.2022.
//

#include "SeaVisionBaseException.hpp"

namespace SeaVision {

SeaVisionException::SeaVisionException(const char *message) noexcept
        : message_(message) {}

SeaVisionException::SeaVisionException(const std::string &message) noexcept
        : SeaVisionException(message.c_str()) {}

const char *SeaVisionException::what() const noexcept { return message_.c_str(); }

}
