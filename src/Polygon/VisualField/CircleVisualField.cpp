//
// Created by leeiozh on 05.08.22.
//

#include <iostream>
#include "CircleVisualField.hpp"

namespace Polygon {

bool CircleVisualField::check_sat(const ScopeState &scope_state,
                                  const Eigen::Vector3d &sat_pos) const {
    Eigen::Vector3d g_vec = sat_pos - scope_state.position;
    // если угол между направлением на спутник и осью визирования меньше угла обзора, засчитываем попадание
    return std::acos(g_vec.dot(scope_state.direction) / g_vec.norm() / scope_state.direction.norm()) <= cone_angle;
}

std::vector<SatState>
CircleVisualField::check_sat_array(const ScopeState &scope_state,
                                   std::vector<SatState> &sat_state, const Eigen::Vector3d &sun_pos) {
    std::vector<SatState> res(0);
    for (auto &sat: sat_state) {
        if (check_sat(scope_state, sat.position3d)) {
            double dir_norm2 = scope_state.direction.norm() * scope_state.direction.norm();
            Eigen::Vector3d oy = (scope_state.position - scope_state.position.dot(scope_state.direction) / dir_norm2 *
                                                         scope_state.direction).normalized();
            Eigen::Vector3d ox = scope_state.direction.normalized().cross(oy);
            Eigen::Vector3d diff = sat.position3d - scope_state.position;
            Eigen::Vector3d proj = diff - diff.dot(scope_state.direction) / dir_norm2 * scope_state.direction;
            sat.position2d = {proj.dot(ox) / std::tan(angle_sizes.x()), proj.dot(oy) / std::tan(angle_sizes.y())};
            sat.position2d /= scope_state.direction.norm();

            // угол солнце - аппарат - телескоп
            double xi = std::acos(
                    (sat.position3d - sun_pos).dot(scope_state.position - sun_pos) / (sat.position3d - sun_pos).norm() /
                    (scope_state.position - sun_pos).norm());
            double dist = (scope_state.position - sat.position3d).norm();

            // яркость спутника
            sat.bright = SUN_TSI * sat.radius * sat.radius / dist / dist * calc_jp(xi);
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

        // вектор направлений на левый нижний и верхний правый угол
        Eigen::Vector4d min_max = Eigen::Vector4d::Zero();

        for (int i = 0; i < 4; i++) {
            min_max[i] = rad * std::stod(line.substr(15 + 7 * i, 6));
        }

        counter_ex++;

        for (int i = 0; i < 4; ++i) {
            Eigen::Vector3d g_vec = {std::cos(min_max[2 + i / 2]) * std::cos(min_max[i % 2]),
                                     std::cos(min_max[2 + i / 2]) * std::sin(min_max[i % 2]),
                                     std::sin(min_max[2 + i / 2])};

            if (std::acos(g_vec.dot(scope_norm)) < cone_angle) {
                res.push_back(counter_ex);
                break;
            }
        }
    }

    file.close();
    return res;
}

std::vector<Star>
CircleVisualField::view_star_array(const ScopeState &scope_state) const {

    // вектор номеров зон, которых затронул конус обзора
    std::vector<int> areas = view_area_nums(scope_state.direction);

    std::vector<Star> res(12121 * static_cast<int>(areas.size())); // 12121 - максимальное число звезд в одной зоне

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

    double dir_norm2 = scope_state.direction.norm() * scope_state.direction.norm();
    Eigen::Vector3d oy = (scope_state.position - scope_state.position.dot(scope_state.direction) / dir_norm2 *
                                                 scope_state.direction).normalized();
    Eigen::Vector3d ox = scope_state.direction.normalized().cross(oy);

    // этот цикл для основного файла
    while (std::getline(file_cat, line)) {
        auto area_num = std::stoi(line.substr(0, 4));

        if (std::find(areas.begin(), areas.end(), area_num) != areas.end()) {

            try {
                double asc = rad * std::stod(line.substr(15, 12));
                double dec = rad * std::stod(line.substr(28, 12));

                // вектор направления на звезду
                Eigen::Vector3d g_vec = {std::cos(dec) * std::cos(asc), std::cos(dec) * std::sin(asc), std::sin(dec)};

                if (std::acos(g_vec.dot(scope_state.direction.normalized())) < cone_angle) {
                    double mag = std::stod(line.substr(110, 6));
                    int in_area_num = std::stoi(line.substr(5, 5));
                    int in_star_com = std::stoi(line.substr(11, 1));

                    Eigen::Vector3d diff = g_vec - scope_state.direction;
                    Eigen::Vector3d proj = diff - diff.dot(scope_state.direction) / dir_norm2 * scope_state.direction;

                    Eigen::Vector2d pos2d = {proj.dot(ox) / std::tan(angle_sizes.x()),
                                             proj.dot(oy) / std::tan(angle_sizes.y())};
                    pos2d /= scope_state.direction.norm();

                    // вычисление яркости звезды по формуле Погсона
                    // TODO: ВОЗМОЖНО, LOG ЛУЧШЕ POW, ПОСОВЕТОВАТЬСЯ
                    double bright = STAR_BRIGHT * std::pow(10, -0.4 * (mag - STAR_MAG));
                    res[counter_ex] = Star{area_num, in_area_num, in_star_com, asc, dec, mag, bright, pos2d};
                    counter_ex++;
                }
            } catch (...) {
                continue;
            }
        }
    }

    // этот цикл для дополнительного файла с более яркими звездами
    while (std::getline(file_sup, line)) {

        auto area_num = std::stoi(line.substr(0, 4));

        if (std::find(areas.begin(), areas.end(), area_num) != areas.end()) {

            try {
                double asc = rad * std::stod(line.substr(15, 12));
                double dec = rad * std::stod(line.substr(28, 12));

                // вектор направления на звезду
                Eigen::Vector3d g_vec = {std::cos(dec) * std::cos(asc), std::cos(dec) * std::sin(asc), std::sin(dec)};

                if (std::acos(g_vec.dot(scope_state.direction.normalized())) < cone_angle) {
                    double mag = std::stod(line.substr(83, 6));
                    int in_area_num = std::stoi(line.substr(5, 5));
                    int in_star_com = std::stoi(line.substr(11, 1));

                    Eigen::Vector3d diff = g_vec - scope_state.direction;
                    Eigen::Vector3d proj = diff - diff.dot(scope_state.direction) / dir_norm2 * scope_state.direction;

                    Eigen::Vector2d pos2d = {proj.dot(ox) / std::tan(angle_sizes.x()),
                                             proj.dot(oy) / std::tan(angle_sizes.y())};
                    pos2d /= scope_state.direction.norm();

                    // вычисление яркости звезды по формуле Погсона
                    double bright = STAR_BRIGHT * std::pow(10, -0.4 * (mag - STAR_MAG));
                    res[counter_ex] = Star{area_num, in_area_num, in_star_com, asc, dec, mag, bright, pos2d};
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