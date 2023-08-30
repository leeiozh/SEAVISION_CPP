//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_STRUCTURES_HPP
#define SEAVISION_STRUCTURES_HPP

#include <Eigen/Dense>
#include <winsock2.h>
#include "SeaVision/Consts.hpp"

namespace SeaVision {

struct VCO {
    double k_num;
    double omega;
    double sigma;

    [[nodiscard]] inline double calc_up() const {
        return k_num * sigma * sigma * (omega - std::sqrt(G_COEFF * k_num));
    }
    [[nodiscard]] inline double calc_down() const {
        return k_num * k_num * sigma * sigma;
    }
};

struct WaveStruct {
    Eigen::Vector3d m0 = Eigen::Vector3d::Zero();
    Eigen::Vector3d vcosalpha = Eigen::Vector3d::Zero();
    Eigen::Vector3d peak_period = Eigen::Vector3d::Zero();
    Eigen::VectorXd freq_spec = Eigen::VectorXd::Zero(FOUR_NUM);
};

struct InputPRLI {
    double step; // difference between neighbour cells in meters
    int pulse;   // 0 = none, 1 = SP1, 2 = MP1, 3 = LP1
    Eigen::MatrixXi bcksctr = Eigen::MatrixXi::Zero(AREA_READ_DIST_PX, AREA_AZIMUTH_PX); // read backscatter in polar
};

struct InputNavi {
    double cog = 0.;
    double sog = 0.;
    double lat = 0.;
    double lon = 0.;
    double spd = 0.;
    double hdg = 0;
};

struct InputStructure {
    InputNavi navi;
    InputPRLI prli;
};

struct ReadParameters {
    int line_start = 0; // start of reading (px)
    int line_end = AREA_READ_DIST_PX; // end of reading (px), if "0" then read to end of file
    int line_size = AREA_READ_DIST_PX; // (px)
    int size_angle = AREA_AZIMUTH_PX; // pixels
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

struct SocketParams {
    SOCKET socket_descriptor;
    sockaddr_in server_address;
};

} // namespace

#endif //SEAVISION_STRUCTURES_HPP
