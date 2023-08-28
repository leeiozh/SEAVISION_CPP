//
// Created by leeiozh on 6/21/23.
//

#ifndef SEAVISION_DISPERSIONDIRECT_HPP
#define SEAVISION_DISPERSIONDIRECT_HPP

#include <Eigen/Dense>
#include <fftw3.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/Math.hpp"

namespace SeaVision {

class DispersionDirect {
    /**
     * class for function of search zone with the clearest signal by calculating deviation of set backscatters
     */
private:
    double curr_az = 0.; // current azimuth of zone with max deviation (degrees)
    int curr_az_ind = 0; // index of zone with max deviation (from 0 to az_zone)
    int az_zone = NUM_AREA; // number of zones split by azimuth

    Eigen::VectorXi curr_std = Eigen::VectorXi::Zero(NUM_SYSTEMS); // current index of zone with max deviation of lines (from 0 to az_zone)
    Eigen::VectorXd curr_len = Eigen::VectorXd::Zero(NUM_SYSTEMS); // current length
    Eigen::VectorXd rose = Eigen::VectorXd::Zero(NUM_AREA); // wind rose with deviations of lines of different zones

public:

    /**
     * constructor
     * @param az_zone number of zones split by azimuth
     */
    explicit DispersionDirect(int az_zone);

    /**
     * function of search a zone with the clearest signal by calculating deviation of set backscatters
     * @param data vector of backscatters
     * @return azimuth of the clearest signal
     */
    int search_area(const std::vector<Eigen::MatrixXi> &data);

    /**
     * function of search a zones with the most vertical wave crests
     * @param data last backscatter
     * @return index of zone with the most vertical wave crests
     */
    [[nodiscard]] Eigen::VectorXi calc_directions(const Eigen::MatrixXi &data, int index);

    /**
     * if current direction change significantly we go to new direction step by step
     * @param new_vec vector of new directions
     */
    void smooth_change(const Eigen::VectorXi& new_vec);

    /**
     * calculating a wave length using 1d fourier transform
     * @param vec profile of wave calculated using vertical integrating
     * @return length
     */
    Eigen::VectorXd calc_length(const Eigen::MatrixXi &data);

    /**
     * getter of index of zone with the clearest signal
     * @return index of zone with the clearest signal
     */
    [[nodiscard]] int get_curr_az_ind() const;

    /**
     * getter of index of zone with the most vertical wave crests
     * @return index of zone with the most vertical wave crests
     */
    [[nodiscard]] Eigen::VectorXi get_curr_std() const;

    /**
     * getter of wind rose with coefficient of verticality wave crests
     * @return wind rose with coefficient of verticality wave crests
     */
    [[nodiscard]] Eigen::VectorXd get_rose() const;

    [[nodiscard]] Eigen::VectorXd get_swh() const;

    [[nodiscard]] Eigen::VectorXd get_len() const;

    [[nodiscard]] Eigen::VectorXd get_dirs() const;
};

} // namespace

#endif //SEAVISION_DISPERSIONDIRECT_HPP
