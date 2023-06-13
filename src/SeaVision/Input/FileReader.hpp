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

    InputStructure read_one_file(std::string file_name);

};

} // namespace
#endif //SEAVISION_FILEREADER_HPP
