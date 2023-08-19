//
// Created by kuznetsov on 04.02.2022.
//

#ifndef SEAVISION_EXCEPTION_HPP
#define SEAVISION_EXCEPTION_HPP

#include <exception>
#include <sstream>
#include <string>


namespace SeaVision {

class SeaVisionException : public std::exception {

protected:

    std::string message_;

public:

    /** constructor for c++ strings
       *  @param message The error message.
     */
    explicit SeaVisionException(const std::string &message) noexcept;

    /** Constructor for c strings
       *  @param message C-style string error message
     */
    explicit SeaVisionException(const char *message) noexcept;

    /** Returns a pointer to the (constant) error description.
       *  @return A pointer to a const char*. The underlying memory
       *          is in possession of the Exception object. Callers must
       *          not attempt to free the memory.
     */
    [[nodiscard]] const char *what() const noexcept override;
};

} // namespace SeaVision

#endif // SEAVISION_EXCEPTION_HPP
