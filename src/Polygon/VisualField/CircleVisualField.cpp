//
// Created by leeiozh on 05.08.22.
//
#include "../Utility/Consts.hpp"
#include <fstream>
#include <iostream>
#include "CircleVisualField.hpp"

namespace Polygon {

bool CircleVisualField::check_sat(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                                  const Eigen::Vector3d &sat_pos) const {
    Eigen::Vector3d g_vec = sat_pos - scope_pos;
    return std::acos(g_vec.dot(scope_dir) / g_vec.norm() / scope_dir.norm()) <= cone_angle;
}

std::vector<sat_state>
CircleVisualField::check_sat_array(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                                   const std::vector<sat_state> &sat_state) const {
    std::vector<sat_state> res(0);
    for (auto &sat: sat_state) {
        if (check_sat(scope_dir, scope_pos, sat.position)) {
            res.push_back(sat);
        }
    }
    return res;
}

std::vector<int>
CircleVisualField::view_area_nums(const Eigen::Vector3d &scope_dir) const {

    std::vector<int> res(0);
    Eigen::Vector3d scope_norm = scope_dir.normalized();

    std::ifstream file;
    file.open(path_to_res + "/index.dat");

    if (!file.is_open()) {
        std::stringstream buff;
        buff << "Файл не открывается, проверьте путь к его папке! Текущий путь: "
             << path_to_res << "/index.dat" << std::endl;
        throw PolygonBaseException(buff.str().c_str());
    }

    std::string line;
    int counter_ex = 0;

    while (std::getline(file, line)) {

        Eigen::Vector4d min_max = Eigen::Vector4d::Zero();

        for (int i = 0; i < 4; i++) {
            min_max[i] = rad * std::stod(line.substr(15 + 7 * i, 6).c_str());
        }

        counter_ex++;
        double min = M_PI;

        for (int i = 0; i < 4; ++i) {
            Eigen::Vector3d g_vec = {std::cos(2 + i / 2) * std::cos(i % 2), std::cos(2 + i / 2) * std::sin(i % 2),
                                     std::sin(2 + i / 2)};
            min = std::min(min, g_vec.dot(scope_norm));
        }

        if (min < cone_angle) {
            res.push_back(counter_ex);
        }
    }

    file.close();
    return res;
}

std::vector<star>
CircleVisualField::view_star_array(const Eigen::Vector3d &scope_dir) const {

    std::vector<int> areas = view_area_nums(scope_dir);

    std::vector<star> res(12121 * static_cast<int>(areas.size()));

    std::ifstream file_cat;
    file_cat.open(path_to_res + "/catalog.dat");

    std::ifstream file_sup;
    file_sup.open(path_to_res + "/suppl_1.dat");

    if (!file_cat.is_open()) {
        std::stringstream buff;
        buff << "Файл не открывается, проверьте путь к его папке! Текущий путь: "
             << path_to_res << "/catalog.dat" << std::endl;
        throw PolygonBaseException(buff.str().c_str());
    }

    if (!file_sup.is_open()) {
        std::stringstream buff;
        buff << "Файл не открывается, проверьте путь к его папке! Текущий путь: "
             << path_to_res << "/suppl_1.dat" << std::endl;
        throw PolygonBaseException(buff.str().c_str());
    }

    int counter_ex = 0;
    std::string line;

    while (std::getline(file_cat, line)) {
        auto area_num = std::stoi(line.substr(0, 4));

        if (std::find(areas.begin(), areas.end(), area_num) != areas.end()) {

            try {
                double asc = rad * std::stod(line.substr(15, 12));
                double dec = rad * std::stod(line.substr(28, 12));

                Eigen::Vector3d g_vec = {std::cos(dec) * std::cos(asc), std::cos(dec) * std::sin(asc), std::sin(dec)};

                if (std::acos(g_vec.dot(scope_dir) / scope_dir.norm()) < cone_angle) {
                    double mag = std::stod(line.substr(110, 6));
                    int in_area_num = std::stoi(line.substr(5, 5));
                    int in_star_com = std::stoi(line.substr(11, 1));
                    res[counter_ex] = star{area_num, in_area_num, in_star_com, asc, dec, mag};
                    counter_ex++;
                }
            } catch (...) {
                continue;
            }
        }
    }

    while (std::getline(file_sup, line)) {
        auto area_num = std::stoi(line.substr(0, 4));

        if (std::find(areas.begin(), areas.end(), area_num) != areas.end()) {

            try {
                double asc = rad * std::stod(line.substr(15, 12));
                double dec = rad * std::stod(line.substr(28, 12));

                Eigen::Vector3d g_vec = {std::cos(dec) * std::cos(asc), std::cos(dec) * std::sin(asc), std::sin(dec)};

                if (std::acos(g_vec.dot(scope_dir) / scope_dir.norm()) < cone_angle) {
                    double mag = std::stod(line.substr(83, 6));
                    int in_area_num = std::stoi(line.substr(5, 5));
                    int in_star_com = std::stoi(line.substr(11, 1));
                    res[counter_ex] = star{area_num, in_area_num, in_star_com, asc, dec, mag};
                    counter_ex++;
                }
            } catch (...) {
                continue;
            }
        }
    }

    res.resize(counter_ex);
    return res;
}

} // namespace Polygon