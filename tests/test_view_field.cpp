//
// Created by leeiozh on 12.02.2022.
//
#include "gtest/gtest.h"
#include "../src/Polygon/VisualField/CircleVisualField.hpp"
#include "../src/Polygon/Utility/Consts.hpp"

TEST(VIEW_SAT, VIEW_SAT_SIMPLE) {

    Polygon::CircleVisualField field(M_PI_2);

    Eigen::Vector3d scope_dir = {1, 0.001, 0};
    Eigen::Vector3d scope_pos = {6370e3, 0, 0};

    std::vector<Polygon::sat_state> sat_state(2);
    sat_state[0] = Polygon::sat_state{10, Eigen::Vector3d(6.8e6, 0, 0), Eigen::Vector3d::Zero()};
    sat_state[1] = Polygon::sat_state{11, Eigen::Vector3d(0, 6.8e6, 0), Eigen::Vector3d::Zero()};

    auto res = field.check_sat_array(scope_dir, scope_pos, sat_state);

    std::cout << res.size() << std::endl;
    for (auto &s: res) {
        std::cout << s.number << std::endl;
    }

}

TEST(FILE, READ_FILE) {

    Polygon::CircleVisualField field(89 * Polygon::rad, "/home/leeiozh/mmcp/polygon/Polygon/resources");

    Eigen::Vector3d scope_dir = {0, 0, -1};
    Eigen::Vector3d scope_pos = {0, 0, -6370e3};
//    Eigen::Vector3d scope_dir = {1, 0, 0};

    auto areas = field.view_area_nums(scope_dir);

//    std::cout << areas.size() << std::endl;
//    for (auto &s: areas) {
//        std::cout << s << std::endl;
//    }

    auto res = field.view_star_array(scope_dir);
    for (auto &r : res){
        if (r.magnitude < 6)
        std::cout << r.ascension << " " << r.declination  << " " << r.magnitude << std::endl;
    }

}