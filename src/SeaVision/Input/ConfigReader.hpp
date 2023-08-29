//
// Created by leeio on 24.08.2023.
//

#ifndef SEAVISION_CONFIGREADER_HPP
#define SEAVISION_CONFIGREADER_HPP

#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "SeaVision/SeaVisionBaseException.hpp"

namespace SeaVision {

class ConfigReader {

private:
    std::string path;
    std::map<std::string, std::string> data;

public:
    explicit ConfigReader(const std::string & path);

    int get_prli_port();

    int get_navi_port();

    int get_serv_port();

    std::string get_ip();

};

} // SeaVision

#endif //SEAVISION_CONFIGREADER_HPP
