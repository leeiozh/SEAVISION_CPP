//
// Created by leeiozh on 16.04.23.
//

#include <iostream>
#include "DispersionCurve.hpp"


namespace SeaVision {

DispersionCurve::DispersionCurve(int max_index, int width, int cut_index) : max_index(max_index), width(width),
                                                                            cut_index(cut_index) {
    data_fourier.resize(max_index);
}

void DispersionCurve::update(const int index, const Eigen::MatrixXd &data) {
    data_fourier[index] = calc_fourier_2d(data);
}

Eigen::MatrixXcd DispersionCurve::calc_fourier_2d(const Eigen::MatrixXd &data) const {

    fftw_complex *f_pic;
    f_pic = (fftw_complex *) malloc(data.size() * sizeof(fftw_complex));

    fftw_complex *pic_fftw;
    pic_fftw = (fftw_complex *) malloc(data.size() * sizeof(fftw_complex));

    fftw_plan plan = fftw_plan_dft_2d(data.rows(), data.cols(), pic_fftw, f_pic, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            pic_fftw[data.cols() * i + j][0] = data(i, j);
            pic_fftw[data.cols() * i + j][1] = 0.;
        }
    }

    fftw_execute(plan);

    Eigen::MatrixXcd res = Eigen::MatrixXd::Zero(cut_index, cut_index);

    for (int i = 0; i < cut_index; ++i) {
        for (int j = 0; j < cut_index; ++j) {
            res(i, j) = std::complex<double>(f_pic[i * data.rows() + j][0], f_pic[i * data.rows() + j][1]);
        }
    }

    fftw_destroy_plan(plan);
    fftw_cleanup();
    fftw_free(f_pic);
    fftw_free(pic_fftw);

    return res;
}

Eigen::MatrixXd DispersionCurve::calc_welch() const {

}


}