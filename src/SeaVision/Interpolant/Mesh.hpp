//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_MESH_HPP
#define SEAVISION_MESH_HPP

#include "Cell.hpp"
#include "Area.hpp"
#include <eigen3/Eigen/Dense>

namespace SeaVision {

class Mesh {
protected:
    Eigen::MatrixX<Cell> mesh;
    double dist_step; // meters
    int size_dist; // pixels
    int size_angle; // pixels


public:
    Mesh(const int &size_dist, const int &size_angle, const double &dist_step);

    [[nodiscard]] Eigen::MatrixXd calc_back(const Area &area, const Eigen::MatrixXi &back) const;

};

}


#endif //SEAVISION_MESH_HPP
