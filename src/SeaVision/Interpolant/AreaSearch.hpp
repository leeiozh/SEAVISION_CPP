//
// Created by leeiozh on 6/21/23.
//

#ifndef SEAVISION_AREASEARCH_HPP
#define SEAVISION_AREASEARCH_HPP

#include <Eigen/Dense>
#include "SeaVision/Input/InputStructure.hpp"
#include "SeaVision/MainProcess/Math.hpp"

namespace SeaVision {

class AreaSearch {
    /**
     * class for function of search zone with the clearest signal by calculating deviation of set backscatters
     */
private:
    double curr_az; // current azimuth of zone with max deviation (degrees)
    int curr_az_ind; // index of zone with max deviation (from 0 to az_zone)
    int az_zone; // number of zones split by azimuth

    int curr_std; // current index of zone with max deviation of lines (from 0 to az_zone)
    Eigen::VectorXd rose; // wind rose with deviations of lines of different zones

public:

    /**
     * constructor
     * @param az_zone number of zones split by azimuth
     */
    explicit AreaSearch(int az_zone);

    /**
     * function of search a zone with the clearest signal by calculating deviation of set backscatters
     * @param data vector of backscatters
     * @return azimuth of the clearest signal
     */
    int search_area(const std::vector<Eigen::MatrixXi> &data);

    /**
     * function of search a zone with the most vertical wave crests
     * @param data last backscatter
     * @return index of zone with the most vertical wave crests
     */
    int search_dir(const Eigen::MatrixXi &data);

    /**
     * getter of index of zone with the clearest signal
     * @return index of zone with the clearest signal
     */
    [[nodiscard]] int get_curr_az_ind() const;

    /**
     * getter of index of zone with the most vertical wave crests
     * @return index of zone with the most vertical wave crests
     */
    [[nodiscard]] int get_curr_std() const;

    /**
     * getter of wind rose with coefficient of verticality wave crests
     * @return wind rose with coefficient of verticality wave crests TODO convert to SWH !!!
     */
    [[nodiscard]] Eigen::VectorXd get_rose() const;
};

} // namespace

#endif //SEAVISION_AREASEARCH_HPP
