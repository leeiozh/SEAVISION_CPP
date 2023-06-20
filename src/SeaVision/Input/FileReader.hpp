//
// Created by leeiozh on 6/13/23.
//

#ifndef SEAVISION_FILEREADER_HPP
#define SEAVISION_FILEREADER_HPP

#include <string>
#include "InputStructure.hpp"

namespace SeaVision {

class FileReader {

    std::string path;

public:

    explicit FileReader(std::string path);

    [[nodiscard]] InputStructure read_one_file(const std::string &file_name) const;

    [[nodiscard]] std::vector<InputStructure> read_queue_files(int num = INT_MAX) const;

};

} // namespace
#endif //SEAVISION_FILEREADER_HPP
