//
// Created by leeiozh on 6/21/23.
//

#ifndef SEAVISION_AREASEARCH_HPP
#define SEAVISION_AREASEARCH_HPP

#include <Eigen/Dense>
#include "SeaVision/Input/InputStructure.hpp"

namespace SeaVision {

class AreaSearch {
    /**
     * class for function of search zone with the clearest signal by calculating deviation of set backscatters
     */
private:
    double curr_az; // current azimuth of zone with max deviation (degrees)
    int az_zone; // number of zones split by azimuth

public:

    /**
     * constructor
     * @param az_zone number of zones split by azimuth
     */
    explicit AreaSearch(int az_zone);

    /**
     * function of search zone with the clearest signal by calculating deviation of set backscatters
     * @param data set of backscatters
     * @return azimuth of the clearest signal
     */
    double search_area(const std::vector<Eigen::MatrixXd> &data);

    double search_area(const std::vector<InputStructure> &data);

};

} // namespace

#endif //SEAVISION_AREASEARCH_HPP
