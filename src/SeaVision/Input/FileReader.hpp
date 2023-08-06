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
    ReadParameters params;

public:

    /**
     * constructor for reader
     * @param path path to folder with files
     * @param params parameters of reading
     */
    explicit FileReader(std::string path, ReadParameters params);

    /**
     * reading one file in folder
     * @param file_name name of file in folder
     * @return structure with read parameters
     */
    [[nodiscard]] InputStructure read_one_file(const std::string &file_name) const;

    [[nodiscard]] InputStructure read_next_file(int index) const;

    [[nodiscard]] InputStructure read_next_file(const std::string &name, int index) const;

    /**
     * read num files from folder
     * @param num number of reading files (from zero in alphabetical order)
     * @return vector of structures with read parameters
     */
    [[nodiscard]] std::vector <InputStructure> read_queue_files(int num = INT_MAX) const;

};

} // namespace
#endif //SEAVISION_FILEREADER_HPP
