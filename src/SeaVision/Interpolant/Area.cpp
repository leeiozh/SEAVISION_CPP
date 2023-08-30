//
// Created by leeiozh on 13.04.23.
//

#include "Area.hpp"

namespace SeaVision {

Area::Area(const double &size_x, const double &size_y, const double &angle, const double &azim, const double &dist) :
        size_x(size_x), size_y(size_y), angle(angle / 180 * M_PI), azimuth(azim / 180 * M_PI), distance(dist) {}

Eigen::Vector2d Area::get_center_coords() const {
    return distance * Eigen::Vector2d{std::cos(azimuth), std::sin(azimuth)};
}

Eigen::Vector2d Area::get_diag_params() const {
    return Eigen::Vector2d{std::sqrt(size_x * size_x + size_y * size_y), std::atan2(size_x, size_y)};
}

Eigen::Matrix<double, 4, 2> Area::get_vertex_coords() const {

    Eigen::Vector2d diag_beta = get_diag_params();
    Eigen::Vector2d center = get_center_coords();

    double angle_min = diag_beta(1) - angle;
    double angle_plus = diag_beta(1) + angle;

    Eigen::Vector2d lu_rd_tmp = 0.5 * diag_beta(0) * Eigen::Vector2d{-std::cos(angle_plus), std::sin(angle_plus)};
    Eigen::Vector2d ru_ld_tmp = 0.5 * diag_beta(0) * Eigen::Vector2d{std::cos(angle_min), std::sin(angle_min)};

    Eigen::Matrix<double, 4, 2> res;
    res << center[0] + ru_ld_tmp[0], center[1] + ru_ld_tmp[1],
            center[0] - lu_rd_tmp[0], center[1] - lu_rd_tmp[1],
            center[0] - ru_ld_tmp[0], center[1] - ru_ld_tmp[1],
            center[0] + lu_rd_tmp[0], center[1] + lu_rd_tmp[1];

    return res;
}
}