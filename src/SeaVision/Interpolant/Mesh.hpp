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
    /**
     * class for mesh for converting from polar to cartesian
     */
protected:
    Eigen::MatrixX<Cell> mesh; // matrix of mesh
    double dist_step; // meters
    int size_dist; // pixels
    int size_angle; // pixels


public:
    /**
     * constructor of mesh
     * @param size_dist distance in meters between neighbours cells
     * @param size_angle size in pixels of input matrix on x axis
     * @param dist_step size in pixels of input matrix on y axis
     */
    Mesh(const int &size_dist, const int &size_angle, const double &dist_step);

    /**
     * calculating backscatter in cartesian
     * @param area class with parameters of cut zone
     * @param back input backscatter in polar
     * @return backscatter in cartesian
     */
    [[nodiscard]] Eigen::MatrixXd calc_back(const Area &area, const Eigen::MatrixXi &back) const;

};

}


#endif //SEAVISION_MESH_HPP
