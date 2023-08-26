//
// Created by leeio on 24.08.2023.
//

#include "ConfigReader.hpp"
#include "SeaVision/Consts.hpp"

namespace SeaVision {

int NUM_AREA, MEAN;
double TURN_PERIOD, MAX_WAIT_SHOT_TIME;

ConfigReader::ConfigReader(const std::string &path) : path(path) {

    std::ifstream file(path);

    if (!file.is_open()) throw SeaVisionException("Error opening config file " + path);

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty() || line[0] == '#' || line[0] == '/') continue;

        size_t delimiter = line.find('=');

        if (delimiter == std::string::npos) throw SeaVisionException("Invalid line in config file " + line);

        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

        data[key] = value;
    }

    file.close();

    if (data["NUM_AREA"].empty()) {
        std::cout << "No NUM_AREA in config file! I will use default value >> " << NUM_AREA_DEF << std::endl;
        NUM_AREA = NUM_AREA_DEF;
    } else {
        int _num_area = std::stoi(data["NUM_AREA"]);
        if (_num_area < 12 || _num_area > 360) {
            std::cout << "Bad NUM_AREA! NUM_AREA should be from [12, 360]! I will use default value >> " << NUM_AREA_DEF
                      << std::endl;
            NUM_AREA = NUM_AREA_DEF;
        } else {
            NUM_AREA = _num_area;
        }
    }

    if (data["MEAN"].empty()) {
        std::cout << "No MEAN in config file! I will use default value >> " << MEAN_DEF << std::endl;
        MEAN = MEAN_DEF;
    } else {
        int _mean = std::stoi(data["MEAN"]);
        if (_mean < 12 || _mean > 360) {
            std::cout << "Bad MEAN! MEAN should be from [1, 256]! I will use default value >> " << MEAN_DEF
                      << std::endl;
            MEAN = MEAN_DEF;
        } else {
            MEAN = _mean;
        }
    }

    if (data["TURN_PERIOD"].empty()) {
        std::cout << "No TURN_PERIOD in config file! I will use default value >> " << TURN_PERIOD_DEF << std::endl;
        TURN_PERIOD = TURN_PERIOD_DEF;
    } else {
        TURN_PERIOD = std::stoi(data["TURN_PERIOD"]);
    }

    if (data["MAX_WAIT_SHOT_TIME"].empty()) {
        std::cout << "No MAX_WAIT_SHOT_TIME in config file! I will use default value >> " << 2 * TURN_PERIOD
                  << std::endl;
        MAX_WAIT_SHOT_TIME = 2 * TURN_PERIOD;
    } else {
        MAX_WAIT_SHOT_TIME = std::stoi(data["MAX_WAIT_SHOT_TIME"]);
    }
}

std::string ConfigReader::get_ip() {

    std::string ip = !data["server_ip"].empty() ? data["server_ip"] : throw SeaVisionException(
            "No server_ip in config file!");

    return ip;
}

int ConfigReader::get_serv_port() {

    int port = !data["server_port"].empty() ? std::stoi(data["server_port"]) : throw SeaVisionException(
            "No server_port in config file!");;

    return port;
}

int ConfigReader::get_curr_port() {

    int port = !data["current_port"].empty() ? std::stoi(data["current_port"]) : throw SeaVisionException(
            "No server_port in config file!");;

    return port;
}

} // SeaVision