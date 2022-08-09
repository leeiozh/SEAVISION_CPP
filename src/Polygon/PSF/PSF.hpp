//
// Created by leeiozh on 06.08.22.
//

#ifndef POLYGON_PSF_HPP
#define POLYGON_PSF_HPP

#include "AberrationFunction/AberrationFunction.hpp"
#include "DeltaFunction/CircularDeltaFunction.hpp"
#include "TransmissionFunction/ExponentForwardTransmissionFunction.hpp"
#include "../Utility/Structs.hpp"
#include <memory>
#include <fftw3.h>

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class PSF {

    Eigen::Matrix<double, SIZE_X, SIZE_Y> matrix = Eigen::Matrix<double, SIZE_X, SIZE_Y>::Zero();

public:

    PSF(const std::shared_ptr<AberrationFunction> &aber_func, const std::shared_ptr<BaseDeltaFunction> &delta_func,
        const std::shared_ptr<BaseTransmissionFunction> &trans_func) {
        Eigen::Matrix<std::complex<double>, SIZE_X, SIZE_Y> comp_matrix;

//        fftw_complex *com_matrix, *ff_matrix;
//        com_matrix = (fftw_complex* ) fftw_malloc(sizeof (fftw_complex) * SIZE_X * SIZE_Y);
//        ff_matrix = (fftw_complex* ) fftw_malloc(sizeof (fftw_complex) * SIZE_X * SIZE_Y);

        for (int i = 0; i < SIZE_X; ++i) {
            double x = static_cast<double>(i) / SIZE_X;
            for (int j = 0; j < SIZE_Y; ++j) {
                double y = static_cast<double>(j) / SIZE_Y;
                comp_matrix(i, j) = delta_func->calc_delta_func(x, y) * trans_func->calc_trans_func(x, y) * std::exp(
                        static_cast<std::complex<double>>(-2 * M_PI * aber_func->calc_aberration_func(x, y)) *
                        std::complex<double>(0, 1));
            }
        }

        Eigen::Matrix<std::complex<double>, SIZE_X, SIZE_Y> fft_matrix;

        fftw_plan p = fftw_plan_dft_2d(SIZE_X, SIZE_Y, reinterpret_cast<fftw_complex*>(&comp_matrix), reinterpret_cast<fftw_complex*>(&fft_matrix), FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_cleanup();

        for (int i = 0; i < SIZE_X; ++i) {
            for (int j = 0; j < SIZE_Y; ++j) {
                double tmp = std::norm(fft_matrix(i, j));
                matrix(i, j) = tmp * tmp;
            }
        }
    };

    double calc_psf(const double &x, const double &y) {
        return matrix(static_cast<int>(x * SIZE_X), static_cast<int>(y * SIZE_Y));
    }

};

}

#endif //POLYGON_PSF_HPP
