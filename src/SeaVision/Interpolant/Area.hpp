//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_AREA_HPP
#define SEAVISION_AREA_HPP

#include <eigen3/Eigen/Dense>

namespace SeaVision {

    class Area {
    public:
        int size_x; // meters
        int size_y; // meters
        double angle; // radians
        double azimuth; // radians
        double distance; // meters
    public:
        /**
         * class for cut square
         * @param size_x horizontal size
         * @param size_y vertical size
         * @param angle orientation angle between vertical and axes of square
         * @param azimuth angle between north and direction on center of square
         * @param distance distance between radar and center of square
         */
        inline Area(const int &size_x, const int &size_y, const double &angle, const double &azimuth,
                    const double &distance) :
                size_x(size_x), size_y(size_y), angle(angle / 180 * M_PI), azimuth(azimuth / 180 * M_PI),
                distance(distance) {};

        /**
         * getter coordinates of area's center
         * @return coordinates of area's center
         */
        [[nodiscard]] Eigen::Vector2d getCenterCoords() const;

        /**
         * getter parameters of area's diagonal
         * @return parameters of area's diagonal
         */
        [[nodiscard]] Eigen::Vector2d getDiagParams() const;

        /**
         * getter coordinates of area's vertex
         * @return coordinates of area's vertex
         */
        [[nodiscard]] Eigen::Matrix<double, 4, 2> getCoordVertex() const;
    };

}

#endif //SEAVISION_AREA_HPP
