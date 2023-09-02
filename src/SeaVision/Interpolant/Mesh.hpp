//
// Created by leeiozh on 13.04.23.
//

#ifndef SEAVISION_MESH_HPP
#define SEAVISION_MESH_HPP

#include <Eigen/Dense>
#include "Cell.hpp"
#include "Area.hpp"
#include "SeaVision/Structures.hpp"

namespace SeaVision {

class Mesh {
    /**
     * class for mesh for converting from polar to cartesian
     */
protected:
    Eigen::MatrixX<Cell> mesh; // matrix of mesh
    ReadParameters params; // parameters of reading (number of lines)


public:
    /**
     * constructor of mesh
     * @param params parameters of reading
     */
    explicit Mesh(const ReadParameters &params);

    /**
     * calculating backscatter in cartesian
     * @param area class with parameters of cut zone
     * @param back input backscatter in polar
     * @param dist_step input backscatter step
     * @return backscatter in cartesian
     */
    [[nodiscard]] Eigen::MatrixXd calc_back(const Area &area, const Eigen::MatrixXi &back, double dist_step) const;

};

}


#endif //SEAVISION_MESH_HPP
