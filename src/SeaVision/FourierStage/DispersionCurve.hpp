//
// Created by leeiozh on 16.04.23.
//

#ifndef SEAVISION_DISPERSIONCURVE_HPP
#define SEAVISION_DISPERSIONCURVE_HPP

#include <Eigen/Dense>
#include <fftw3.h>
#include "SeaVision/Structures.hpp"
#include "SeaVision/Consts.hpp"
#include "SeaVision/Math.hpp"

namespace SeaVision {

class DispersionCurve {

    /**
     * class for all fourier stage: Fourier and Welch transforms, calculating spectrum parameters
     */

protected:

    WaveStruct spectrum_struct;                 // current spectrum parameters (m0, m1, peak_period, freq_spectrum)
    std::vector<Eigen::MatrixXcd> data_fourier; // array for spectrum calculation (3D)
    Eigen::MatrixXd picture;                    // current curve picture (2D)

public:

    DispersionCurve();

    /**
     * update current state
     * @param index current index
     * @param data current backscatter
     */
    void update(const int &index, const Eigen::MatrixXd &data);

    /**
     * calculating Fourier stage
     * @param data current backscatter
     * @return Fourier transform of current back
     */
    [[nodiscard]] static Eigen::MatrixXcd calc_fourier_2d_one(const Eigen::MatrixXd &data);

    /**
     * calculating Welch transform with half Hanning window
     * @return Welch transform of current FOUR_NUM Fourier queue
     */
    [[nodiscard]] Eigen::VectorX<Eigen::MatrixXd> calc_welch(const int &index) const;

    /**
     * calculating main parameters of dispersion curve and spectrum
     */
    void calc_curve();

    /**
     * process one curve on dispersion portrait
     * @param pic dispersion portrait (this.picture or minus previous signal)
     * @param times times that the function is called (from 0)
     * @return pair arrays of signal and noise
     */
    std::pair<Eigen::MatrixXd, Eigen::MatrixXd> proc_one_curve(const Eigen::MatrixXd &pic, const int &times);

    /**
     * calculating freq / 2pi = \sqrt{gk} + k vcosalpha
     * @param k_num wave number
     * @param vcosalpha coefficient calculated using weighted least squares
     * @return
     */
    static double dispersion_func(double k_num, double vcosalpha);

    /**
     * calculation vcosalpha coefficient using least square method
     * @param vco_vec vector of {k_num_i, omega_i, sigma_i}
     * @param counter true size of vector
     * @return coefficient vcosalpha
     */
    static double calc_vcos(const std::vector<VCO> &vco_vec, int counter);

    /**
     * convert from S(omega, kx, ky) to S(omega, |k|)
     * @return dispersion picture
     */
    [[nodiscard]] static Eigen::MatrixXd calc_abs_wave_num(const Eigen::VectorX<Eigen::MatrixXd> &data3d);

    /**
     * getter of spectrum wave parameters
     * @return structure with zeroth and first momentum, period of spectrum peak, frequency spectrum
     */
    WaveStruct get_params();

};

}

#endif //SEAVISION_DISPERSIONCURVE_HPP
