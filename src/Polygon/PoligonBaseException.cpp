//
// Created by leeiozh on 02.08.2022.
//

#include "PoligonBaseException.hpp"

namespace Polygon {

PolygonBaseException::PolygonBaseException(const char *message) noexcept
        : message_(message) {}

PolygonBaseException::PolygonBaseException(const std::string &message) noexcept
        : PolygonBaseException(message.c_str()) {}

const char *PolygonBaseException::what() const noexcept { return message_.c_str(); }
} // namespace Slae
