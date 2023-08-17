//
// Created by leeiozh on 6/13/23.
//

#include "FileReader.hpp"

namespace SeaVision {

FileReader::FileReader(std::string path, const ReadParameters params) : path(std::move(path)), params(params) {}

InputStructure FileReader::read_one_file(const std::string &file_name) const {

    InputStructure res{0, 0, 0, 0, 0, STEP, params.line_size, 4096};

    std::ifstream file((path + file_name).c_str(), std::ios::in | std::ios::binary);

    if (file.is_open()) {

        uint8_t head[23];
        file.read((char *) &head, sizeof(head));

        // check if SP1 with quality 1.875
        if (head[10] != 7) {
            std::stringstream buff;
            buff << "Not SP1 in " << file_name;
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

        // reading throw one azimuth
        for (int i = 0; i < res.size_az; ++i) {

            // read full line throw current azimuth
            uint8_t curr_line[res.size_az];
            file.read((char *) &curr_line, sizeof(curr_line));

            // read some metadata which we don't used
            uint8_t junk[8];
            file.read((char *) &junk, sizeof(junk));

            // record necessary data

            if (params.line_start == 0 && params.line_end == 0) {
                res.size_dist = res.size_az;
                for (int j = 0; j < res.size_dist; ++j) {
                    res.bcksctr(j, i) = curr_line[j];
                }
            } else {
                for (int j = params.line_start; j < params.line_end; ++j) {
                    res.bcksctr(j - params.line_start, i) = curr_line[j];
                }
            }
        }
        file.close();
    } else {
        std::stringstream buff;
        buff << "Error with opening a file " << file_name;
        throw SeaVisionException(buff.str().c_str());
    }

    return res;
}

InputStructure FileReader::read_next_file(int index) const {
    std::vector<std::filesystem::path> filenames;
    for (const auto &entry: std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file() && std::filesystem::file_size(entry) > 1024) {
            filenames.push_back(entry.path().filename());
        }
    }

    if (index > filenames.size()) {
        std::stringstream buff;
        buff << "Not enough files in " << path << ". Number of files in folder is " << filenames.size()
             << ", but requested index is " << index;
        throw SeaVisionException(buff.str().c_str());
    }

    // sorting in alphabetical order
    std::sort(filenames.begin(), filenames.end(),
              [](const auto &lhs, const auto &rhs) {
                  return lhs.string() < rhs.string();
              });
    return read_one_file(filenames[index].string());
}

InputStructure FileReader::read_next_file(const std::string &name, int index) const {
    std::vector<std::filesystem::path> filenames;
    for (const auto &entry: std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file() && entry.path().filename().string()[0] != '.') {
            filenames.push_back(entry.path().filename());
        }
    }

    // sorting in alphabetical order
    std::sort(filenames.begin(), filenames.end(),
              [](const auto &lhs, const auto &rhs) {
                  return lhs.string() < rhs.string();
              });

    int start = 0;
    int name_int = std::stoi(name.substr(9, 6));
    for (const auto &filename: filenames) {
        if (std::stoi(filename.string().substr(9, 6)) > name_int) {
            break;
        } else {
            start++;
        }
    }

    return read_one_file(filenames[start + index].string());
}

std::vector<InputStructure> FileReader::read_queue_files(const int num) const {

    std::vector<InputStructure> res;

    std::vector<std::filesystem::path> filenames;
    for (const auto &entry: std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file() && std::filesystem::file_size(entry) > 1) {
            filenames.push_back(entry.path().filename());
        }
    }

    // sorting in alphabetical order
    std::sort(filenames.begin(), filenames.end(),
              [](const auto &lhs, const auto &rhs) {
                  return lhs.string() < rhs.string();
              });

    // throw exception if not enough files in folder
    if (num > static_cast<int>(filenames.size())) {
        std::stringstream buff;
        buff << "Not enough file in " << path << ". You request " << num << " files, but there is only " <<
             static_cast<int>(filenames.size());
        throw SeaVisionException(buff.str().c_str());
    }

    for (int i = 0; i < std::min(num, static_cast<int>(filenames.size())); ++i) {
        res.push_back(read_one_file(filenames[i].string()));
    }

    return res;
}

}