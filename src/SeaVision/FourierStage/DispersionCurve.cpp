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
    data_fourier[index] = calc_fourier_2d_one(data);
}

Eigen::MatrixXcd DispersionCurve::calc_fourier_2d_one(const Eigen::MatrixXd &data) const {

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

Eigen::VectorX<Eigen::MatrixXd> DispersionCurve::calc_welch() const {

    Eigen::VectorX<Eigen::MatrixXd> res = Eigen::VectorX<Eigen::MatrixXd>(data_fourier.size());

    for (int t = 0; t < data_fourier.size(); ++t) {
        res[t] = Eigen::MatrixXd::Zero(data_fourier[0].rows(), data_fourier[0].cols());
    }

    for (int i = 0; i < data_fourier[0].rows(); ++i) {
        for (int j = 0; j < data_fourier[0].rows(); ++j) {
            fftw_complex *out;
            out = (fftw_complex *) malloc(data_fourier.size() * sizeof(fftw_complex));

            fftw_complex *inp;
            inp = (fftw_complex *) malloc(data_fourier.size() * sizeof(fftw_complex));

            fftw_plan plan = fftw_plan_dft_1d(static_cast<int>(data_fourier.size()), inp, out, FFTW_FORWARD,
                                              FFTW_ESTIMATE);
            for (int t = 0; t < data_fourier.size(); ++t) {
                inp[t][0] = data_fourier[t](i, j).real();
                inp[t][1] = data_fourier[t](i, j).imag();
            }
            fftw_execute(plan);

            for (int t = 0; t < data_fourier.size(); ++t) {
                res[t](i, j) = std::sqrt(out[t][0] * out[t][0] + out[t][1] * out[t][1]);
            }

            fftw_destroy_plan(plan);
            fftw_cleanup();
            fftw_free(out);
            fftw_free(inp);
        }
    }
    return res;
}

Eigen::MatrixXd DispersionCurve::calc_abs_wave_num(const Eigen::VectorX<Eigen::MatrixXd> &data3d) {

    Eigen::MatrixXd res = Eigen::MatrixXd::Zero(data3d.size(), data3d[0].rows());

    for (int k_x = 0; k_x < data3d[0].rows(); ++k_x) {
        for (int k_y = 0; k_y < data3d[0].cols(); ++k_y) {
            double k_abs = std::sqrt(k_x * k_x + k_y * k_y);
            int k_abs_div = std::floor(k_abs);
            double k_abs_mod = k_abs - k_abs_div;


            bool flag1 = k_abs_div < data3d[0].rows();
            bool flag2 = k_abs_div < (data3d[0].rows() - 1);

            for (int t = 0; t < data3d.size(); ++t) {
                if (flag1) res(t, k_abs_div) += (1 - k_abs_mod) * data3d[t](k_x, k_y);
                if (flag2) res(t, k_abs_div + 1) += k_abs_mod * data3d[t](k_x, k_y);
            }
        }
    }
    return res;
}

} // namespace