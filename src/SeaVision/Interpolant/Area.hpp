//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_AREA_HPP
#define SEAVISION_AREA_HPP

#include <Eigen/Dense>

namespace SeaVision {

class Area {
public:
    double size_x;   // meters
    double size_y;   // meters
    double angle;    // radians
    double azimuth;  // radians
    double distance; // meters
public:
    Area() = default;

    /**
     * class for cut square
     * @param size_x horizontal size (meters)
     * @param size_y vertical size   (meters)
     * @param angle orientation angle between vertical and axes of square (degrees)
     * @param azimuth angle between north and direction on center of square (degrees)
     * @param distance distance between radar and center of square (meters)
     */

    Area(const double &size_x, const double &size_y, const double &angle, const double &azim, const double &dist);

    [[nodiscard]] Eigen::Vector2d get_center_coords() const;

    [[nodiscard]] Eigen::Vector2d get_diag_params() const;

    [[nodiscard]] Eigen::Matrix<double, 4, 2> get_vertex_coords() const;
};

}

#endif //SEAVISION_AREA_HPP
