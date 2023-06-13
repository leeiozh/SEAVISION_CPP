//
// Created by leeiozh on 6/13/23.
//
#include "iostream"
#include <fstream>
#include <utility>
#include "FileReader.hpp"

namespace SeaVision {

FileReader::FileReader(std::string path) : path(std::move(path)) {}

InputStructure FileReader::read_one_file(std::string file_name) {

    InputStructure res;

    std::ifstream file((path + file_name).c_str());

    for (std::string line; std::getline(file, line);) {
        std::cout << std::stoi(line) << std::endl;
    }

    return res;

}

}