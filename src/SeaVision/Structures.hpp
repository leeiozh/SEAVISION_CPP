//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_STRUCTURES_HPP
#define SEAVISION_STRUCTURES_HPP

#include <Eigen/Dense>
#include "SeaVision/Consts.hpp"

namespace SeaVision {

struct WaveStruct {
    Eigen::Vector3d m0 = Eigen::Vector3d::Zero();
    Eigen::Vector3d vcosalpha = Eigen::Vector3d::Zero();
    Eigen::Vector3d peak_period = Eigen::Vector3d::Zero();
    Eigen::VectorXd freq_spec = Eigen::VectorXd::Zero(FOUR_NUM);
};

struct InputPRLI {
    double step; // difference between neighbour cells in meters
    int size_dist = 4096; // AREA_DISTANCE_PX; // number of read lines (distance)
    int size_az = 4096; //AREA_AZIMUTH_PX; // number of read rows (azimuths)
    Eigen::MatrixXi bcksctr = Eigen::MatrixXi::Zero(size_dist, size_az); // read backscatter in polar
};

struct InputConditions {
    double cog = 0.;
    double sog = 0.;
    double lat = 0.;
    double lon = 0.;
    double spd = 0.;
    double hdg = 0;
};

struct InputStructure {
    InputConditions cond;
    InputPRLI prli;
};

struct ReadParameters {
    int line_start = 0; // start of reading (px)
    int line_end = 0; // end of reading (px), if "0" then read to end of file
    int line_size = line_end - line_start; // (px)
    int size_angle = 4096; // pixels
};


struct OutputStructure {

    Eigen::Vector3d swh = Eigen::Vector3d::Zero();
    Eigen::Vector3d per = Eigen::Vector3d::Zero();
    Eigen::Vector3d dir = Eigen::Vector3d::Zero();
    Eigen::Vector3d len = Eigen::Vector3d::Zero();

    Eigen::Vector3d m0 = Eigen::Vector3d::Zero(); // DELETE!!!
    Eigen::Vector3d vcos = Eigen::Vector3d::Zero(); // DELETE!!!

    Eigen::VectorXd freq_spec = Eigen::VectorXd::Zero(FOUR_NUM);
    Eigen::VectorXd rose = Eigen::VectorXd::Zero(NUM_AREA);

};

} // namespace

#endif //SEAVISION_STRUCTURES_HPP
