//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_INPUTSTRUCTURE_HPP
#define SEAVISION_INPUTSTRUCTURE_HPP

#include <Eigen/Dense>

namespace SeaVision {

struct InputStructure {
    double cog, sog, giro;
    double lat, lon;
    double step; // difference between neighbour cells in meters
    int size_dist = 464; // number of read lines (distance)
    int size_az = 4096; // number of read rows (azimuths)
    Eigen::MatrixXi bcksctr = Eigen::MatrixXi::Zero(size_dist, size_az); // read backscatter in polar
};

struct ReadParameters {
    int line_start = 0; // start of reading (px)
    int line_end = 0; // end of reading (px), if "0" then read to end of file
    int line_size = line_end - line_start; // (px)
    int size_angle = 4096; // pixels
};
}

#endif //SEAVISION_INPUTSTRUCTURE_HPP
