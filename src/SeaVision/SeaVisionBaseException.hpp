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
    /* Базовый класс исключения в пакетах С++ */
protected:
    /* Поля класса */
    std::string message_;

public:
    /* Конструкторы класса */
    /** Constructor (C++ STL strings).
       *  @param message The error message.
     */
    explicit SeaVisionException(const std::string &message) noexcept;

    /** Constructor (C strings).
       *  @param message C-style string error message.
       *                 The string contents are copied upon construction.
       *                 Hence, responsibility for deleting the char* lies
       *                 with the caller.
     */
    explicit SeaVisionException(const char *message) noexcept;

    /*  Методы класса */
    /** Returns a pointer to the (constant) error description.
       *  @return A pointer to a const char*. The underlying memory
       *          is in posession of the Exception object. Callers must
       *          not attempt to free the memory.
     */
    const char *what() const noexcept override;
};

} // namespace SeaVision

#endif // SEAVISION_EXCEPTION_HPP
