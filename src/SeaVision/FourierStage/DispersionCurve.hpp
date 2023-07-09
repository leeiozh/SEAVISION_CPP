//
// Created by leeiozh on 16.04.23.
//

#ifndef SEAVISION_DISPERSIONCURVE_HPP
#define SEAVISION_DISPERSIONCURVE_HPP

#include "SpectrumStructure.hpp"
#include "SeaVision/Consts.hpp"
#include <Eigen/Dense>
#include <fftw3.h>

namespace SeaVision {

class DispersionCurve {

    /**
     * class for all fourier stage: Fourier and Welch transforms, calculating spectrum parameters
     */

protected:
    int width = DELTA_FREQ; // width of cut curve
    int cut_index = CUT_NUM; // size of trimmed array after fourier transform
    int max_index = FOUR_NUM; // size of array for spectrum calculation
    double max_wave_num = K_MAX; // maximum wave number

    SpectrumStruct spectrum_struct; // spectrum parameters (m0, m1, peak_period, freq_spectrum)

    std::vector<Eigen::MatrixXcd> data_fourier; // array for spectrum calculation (3D)
    Eigen::MatrixXd picture; // current curve picture (2D)

public:

    /**
     * constructor
     * @param max_index size of array for spectrum calculation
     * @param width width of cut curve
     * @param cut_index size of trimmed array after fourier transform
     */
    DispersionCurve(int max_index, int width, int cut_index, double max_wave_num);

    /**
     * update current state
     * @param index current index
     * @param data current backscatter
     */
    void update(int index, const Eigen::MatrixXd &data);

    /**
     * calculating Fourier stage
     * @param data current backscatter
     * @return Fourier transform of current back
     */
    [[nodiscard]] Eigen::MatrixXcd calc_fourier_2d_one(const Eigen::MatrixXd &data) const;

    /**
     * calculating Welch transform with half Hanning window
     * @return Welch transform of current Fourier queue
     */
    [[nodiscard]] Eigen::VectorX<Eigen::MatrixXd> calc_welch(int index) const;

    /**
     * calculating main parameters of dispersion curve and spectrum
     */
    void calc_curve();

    /**
     * calculating coefficient in \omega = \sqrt{gk} + k vcosalpha using weighted least squares
     * @param omega vector of frequencies
     * @param k_num vector of wave numbers
     * @return coefficient
     */
    static double calc_vcosalpha(const std::vector<double> &omega, const std::vector<double> &k_num,
                                 const std::vector<double> &sigma);

    /**
     * calculating freq / 2pi = \sqrt{gk} + k vcosalpha
     * @param k_num wave number
     * @param vcosalpha coefficient calculated using weighted least squares
     * @return
     */
    static double dispersion_func(double k_num, double vcosalpha);

    /**
     * convert from S(omega, kx, ky) to S(omega, |k|)
     * @return dispersion picture
     */
    [[nodiscard]] static Eigen::MatrixXd calc_abs_wave_num(const Eigen::VectorX<Eigen::MatrixXd> &data3d);

    /**
     * getter of spectrum wave parameters
     * @return structure with zeroth and first momentum, period of spectrum peak, frequency spectrum
     */
    SpectrumStruct get_params();

};

}

#endif //SEAVISION_DISPERSIONCURVE_HPP
