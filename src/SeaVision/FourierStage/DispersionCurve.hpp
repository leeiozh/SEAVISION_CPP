//
// Created by leeiozh on 16.04.23.
//

#ifndef SEAVISION_DISPERSIONCURVE_HPP
#define SEAVISION_DISPERSIONCURVE_HPP

#include "SeaVision/MainProcess/Consts.hpp"
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

    double m0 = 0.;
    double m1 = 0.;
    double peak_period = 0.;
    Eigen::VectorXd freq_spec = Eigen::VectorXd::Zero(FOUR_NUM);

    Eigen::VectorX<Eigen::MatrixXcd> data_fourier; // array for spectrum calculation
    Eigen::MatrixXd picture; // current curve picture

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
    [[nodiscard]] Eigen::VectorX<Eigen::MatrixXd> calc_welch() const;

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
};

}

#endif //SEAVISION_DISPERSIONCURVE_HPP
