//
// Created by leeiozh on 12.02.2022.
//
#include "gtest/gtest.h"
#include "../src/Polygon/VisualField/CircleVisualField.hpp"
#include "../src/Polygon/Utility/Consts.hpp"

namespace Polygon {

TEST(VIEW_SAT, VIEW_SAT_SIMPLE) {

    const int SIZE_X = 1000;
    const int SIZE_Y = 1000;

    Polygon::NoiseParams noiseParams{3, 3};
    const Polygon::MatrixParams matrixParams{1e-5, 1e-5, 1e-5 * SIZE_X, 1e-5 * SIZE_Y, noiseParams, 100000};

    Polygon::CircleVisualField field(2. / deg, Eigen::Vector2d(matrixParams.matr_x, matrixParams.matr_y),
                                     "/home/leeiozh/mmcp/polygon/Polygon/resources");

    Eigen::Vector3d scope_dir = {0, 1.0, 0};
    Eigen::Vector3d scope_pos = {0, 6370e3, 0};
    Polygon::ScopeState scope_state = {scope_pos, scope_dir};
    Eigen::Vector3d sun_pos = {0, 1.5e11, 0.};

    std::vector<Polygon::SatState> sat_state(2);
    sat_state[0] = Polygon::SatState{10, 0, Eigen::Vector3d(6.8e6, 0, 0)};
    sat_state[1] = Polygon::SatState{11, 0, Eigen::Vector3d(0, 6.8e6, 0)};

    auto res = field.check_sat_array(scope_state, sat_state, sun_pos);

    for (auto &s: res) {
        std::cout << s.number << std::endl;
    }

}

TEST(FILE, READ_FILE) {

    const int SIZE_X = 1000;
    const int SIZE_Y = 1000;

    Polygon::NoiseParams noiseParams{3, 3};
    const Polygon::MatrixParams matrixParams{1e-5, 1e-5, 1e-5 * SIZE_X, 1e-5 * SIZE_Y, noiseParams, 100000};

    Polygon::CircleVisualField field(2. / deg, Eigen::Vector2d(matrixParams.matr_x, matrixParams.matr_y),
                                     "/home/leeiozh/mmcp/polygon/Polygon/resources");

    Eigen::Vector3d scope_dir = {0, -1., 0.};
    Eigen::Vector3d scope_pos = {0, -6370e3, 0.};
//    Eigen::Vector3d scope_dir = {1, 0, 0};

    auto areas = field.view_area_nums(scope_dir);

    std::cout << areas.size() << std::endl;
    for (auto &s: areas) {
        std::cout << s << std::endl;
    }

//    auto res = field.view_star_array(scope_dir);
//    for (auto &r : res){
//        if (r.magnitude < 6)
//        std::cout << r.ascension << " " << r.declination  << " " << r.magnitude << std::endl;
//    }

}
}
