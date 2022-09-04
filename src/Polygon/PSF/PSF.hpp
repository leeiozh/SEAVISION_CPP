//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_PSF_HPP
#define POLYGON_PSF_HPP

#include "AberrationFunction/AberrationFunction.hpp"
#include "DeltaFunction/BaseDeltaFunction.hpp"
#include "TransmissionFunction/BaseTransmissionFunction.hpp"
#include <fftw3.h>

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class PSF {
protected:

    Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);

public:

    inline PSF(const std::shared_ptr<AberrationFunction> &aber_func,
               const std::shared_ptr<BaseDeltaFunction> &delta_func,
               const std::shared_ptr<BaseTransmissionFunction> &trans_func) {

        fftw_complex *comp_matrix;
        comp_matrix = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        for (int i = 0; i < SIZE_X; ++i) {
            double x = 2 * static_cast<double>(i) / (SIZE_X - 1) - 1;
            for (int j = 0; j < SIZE_Y; ++j) {
                double y = 2 * static_cast<double>(j) / (SIZE_Y - 1) - 1;
                auto del = static_cast<std::complex<double>>(delta_func->calc_delta_func(x, y));
                auto trans = static_cast<std::complex<double>>(trans_func->calc_trans_func(x, y));
                double aber = aber_func->calc_aberration_func(x, y);

                std::complex<double> number = del * trans * static_cast<std::complex<double>>(std::exp(
                        -2 * M_PI * aber * std::complex<double>(0, 1)));

                comp_matrix[SIZE_X * i + j][0] = number.real();
                comp_matrix[SIZE_X * i + j][1] = number.imag();

            }
        }

        std::cout << comp_matrix[110][0] << " " << comp_matrix[110][1] << std::endl;

        fftw_complex *fft_matrix;
        fft_matrix = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        fftw_plan p = fftw_plan_dft_2d(SIZE_X, SIZE_Y, comp_matrix, fft_matrix, FFTW_BACKWARD, FFTW_ESTIMATE);

        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_cleanup();

        double max_element = 0.;

        for (int i = 0; i < SIZE_X; ++i) {
            for (int j = 0; j < SIZE_Y; ++j) {
                double real = fft_matrix[i * SIZE_X + j][0];
                double imag = fft_matrix[i * SIZE_X + j][1];
                matrix(i, j) = real * real + imag * imag;
                max_element = std::max(max_element, matrix(i, j));
            }
        }
        matrix /= max_element;
    };

    inline double calc_psf_at_point(const double &x, const double &y) {
        return matrix(static_cast<int>((x + 1) * 0.5 * (SIZE_X - 1)), static_cast<int>((y + 1) * 0.5 * (SIZE_Y - 1)));
    }

    inline Eigen::MatrixXd get_matrix() {
        return matrix;
    }

};

}

#endif //POLYGON_PSF_HPP
