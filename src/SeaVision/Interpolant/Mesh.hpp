//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_MESH_HPP
#define SEAVISION_MESH_HPP

#include "Cell.hpp"
#include "Area.hpp"
#include "SeaVision/Input/InputStructure.hpp"
#include <eigen3/Eigen/Dense>

namespace SeaVision {

class Mesh {
    /**
     * class for mesh for converting from polar to cartesian
     */
protected:
    Eigen::MatrixX<Cell> mesh; // matrix of mesh
    ReadParameters params; // parameters of reading (number of lines)
    double dist_step = STEP; // meters


public:
    /**
     * constructor of mesh
     * @param params parameters of reading
     * @param dist_step size in pixels of input matrix on y axis
     */
    explicit Mesh(ReadParameters params, double dist_step = STEP);

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
