//
// Created by leeiozh on 16.04.23.
//

#ifndef SEAVISION_DISPERSIONCURVE_HPP
#define SEAVISION_DISPERSIONCURVE_HPP

#include <Eigen/Dense>
#include <fftw3.h>

namespace SeaVision {

class DispersionCurve {

    /*
     * class for all fourier stage
     */

protected:
    int width; // width of cut curve
    int cut_index; // size of trimmed array after fourier transform
    int max_index; // size of array for spectrum calculation

    Eigen::VectorX<Eigen::MatrixXcd> data_fourier; // array for spectrum calculation

public:

    /**
     * constructor
     * @param max_index size of array for spectrum calculation
     * @param width width of cut curve
     * @param cut_index size of trimmed array after fourier transform
     */
    DispersionCurve(int max_index, int width, int cut_index);

    /**
     * update current state
     * @param index current index
     * @param data current backscatter
     */
    void update(int index, const Eigen::MatrixXd& data);

    /**
     * calculating Fourier stage
     * @param data current backscatter
     * @return fourier transform of current back
     */
    [[nodiscard]] Eigen::MatrixXcd calc_fourier_2d(const Eigen::MatrixXd& data) const;

    [[nodiscard]] Eigen::MatrixXd calc_welch() const;
};

}

#endif //SEAVISION_DISPERSIONCURVE_HPP
