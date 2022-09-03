//
// Created by leeiozh on 05.08.22.
//
#include "../Utility/Consts.hpp"
#include "../Utility/Functions.hpp"
#include <fstream>
#include <iostream>
#include "CircleVisualField.hpp"

namespace Polygon {

bool CircleVisualField::check_sat(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                                  const Eigen::Vector3d &sat_pos) const {
    Eigen::Vector3d g_vec = sat_pos - scope_pos;
    return std::acos(g_vec.dot(scope_dir) / g_vec.norm() / scope_dir.norm()) <= cone_angle;
}

std::vector<SatState>
CircleVisualField::check_sat_array(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                                   std::vector<SatState> &sat_state, const Eigen::Vector3d &sun_pos) {
    std::vector<SatState> res(0);
    for (auto &sat: sat_state) {
        if (check_sat(scope_dir, scope_pos, sat.position3d)) {
            sat.position2d.x() = std::asin(sat.position3d.normalized().dot(Eigen::Vector3d{1, 0, 0}));
            sat.position2d.y() = std::asin(sat.position3d.normalized().dot(Eigen::Vector3d{0, 1, 0}));
            sat.position2d /= cone_angle;
            double xi = std::acos(
                    (sat.position3d - sun_pos).dot(scope_pos - sun_pos) / (sat.position3d - sun_pos).norm() /
                    (scope_pos - sun_pos).norm());
            sat.bright = STAR_BRIGHT * sat.radius * sat.radius / (scope_pos - sat.position3d).norm() /
                         (scope_pos - sat.position3d).norm() * calc_jp(xi);
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
            min_max[i] = rad * std::stod(line.substr(15 + 7 * i, 6));
        }

        counter_ex++;
        double min = M_PI;

        for (int i = 0; i < 4; ++i) {
            Eigen::Vector3d g_vec = {std::cos(min_max[2 + i / 2]) * std::cos(min_max[i % 2]),
                                     std::cos(min_max[2 + i / 2]) * std::sin(min_max[i % 2]),
                                     std::sin(min_max[2 + i / 2])};
            min = std::min(min, g_vec.dot(scope_norm));
        }

        if (min < cone_angle) {
            res.push_back(counter_ex);
        }
    }

    file.close();
    return res;
}

std::vector<Star>
CircleVisualField::view_star_array(const Eigen::Vector3d &scope_dir) const {

    std::vector<int> areas = view_area_nums(scope_dir);

    std::vector<Star> res(12121 * static_cast<int>(areas.size()));

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
                    res[counter_ex] = Star{area_num, in_area_num, in_star_com, asc, dec, mag};
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
                    Eigen::Vector2d position2d = {g_vec.dot(Eigen::Vector3d{1, 0, 0}),
                                                  g_vec.dot(Eigen::Vector3d{0, 1, 0})};
                    double bright = STAR_BRIGHT * std::pow(10, mag - STAR_MAG);
                    res[counter_ex] = Star{area_num, in_area_num, in_star_com, asc, dec, mag, bright, position2d};
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