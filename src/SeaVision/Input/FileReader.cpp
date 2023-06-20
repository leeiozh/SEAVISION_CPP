//
// Created by leeiozh on 6/13/23.
//
#include <filesystem>
#include <fstream>
#include <utility>
#include "FileReader.hpp"
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

FileReader::FileReader(std::string path) : path(std::move(path)) {}

InputStructure FileReader::read_one_file(const std::string &file_name) const {

    InputStructure res;

    std::ifstream file((path + file_name).c_str(), std::ios::in | std::ios::binary);

    if (file.is_open()) {

        uint8_t head[23];
        file.read((char *) &head, sizeof(head));

        if (head[10] != 7) {
            std::stringstream buff;
            buff << "Not SP1 in " << file_name << std::endl;
            throw SeaVisionException(buff.str().c_str());
        }

        file.seekg(0);

        double meta[8];
        file.read((char *) &meta, sizeof(meta));

        res.giro = meta[3];
        res.lat = meta[4];
        res.lon = meta[5];
        res.cog = meta[6];
        res.sog = meta[7];


        for (int i = 0; i < res.size; ++i) {

            uint8_t curr_line[res.size];
            file.read((char *) &curr_line, sizeof(curr_line));

            uint8_t junk[8];
            file.read((char *) &junk, sizeof(junk));

            for (int j = 0; j < res.size; ++j) {
                res.bcksctr(j, i) = curr_line[j];
            }

        }

        file.close();
    } else {
        std::stringstream buff;
        buff << "Error with opening a file " << file_name << std::endl;
        throw SeaVisionException(buff.str().c_str());
    }

    return res;

}

std::vector<InputStructure> FileReader::read_queue_files(const int num) const {

    std::vector<InputStructure> res;

    std::vector<std::filesystem::path> filenames;
    for (const auto &entry: std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file()) {
            filenames.push_back(entry.path().filename());
        }
    }

    std::sort(filenames.begin(), filenames.end(),
              [](const auto &lhs, const auto &rhs) {
                  return lhs.string() < rhs.string();
              });

    for (int i = 0; i < std::min(num, static_cast<int>(filenames.size())); ++i) {
        res.push_back(read_one_file(filenames[i].string()));
    }

    return res;
}

}