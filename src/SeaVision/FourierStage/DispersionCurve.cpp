//
// Created by leeiozh on 16.04.23.
//

#include <iostream>
#include <fstream>
#include "DispersionCurve.hpp"
#include "SeaVision/MainProcess/Math.hpp"


namespace SeaVision {

DispersionCurve::DispersionCurve(int max_index, int width, int cut_index, double max_wave_num) :
        max_index(max_index), width(width), cut_index(cut_index), max_wave_num(max_wave_num) {
    data_fourier.resize(max_index);
}

void DispersionCurve::update(const int index, const Eigen::MatrixXd &data) {
    data_fourier[index] = calc_fourier_2d_one(data);

    if (index >= 255) {
        auto welch = calc_welch();
        picture = calc_abs_wave_num(welch);
        picture /= picture.maxCoeff();
        calc_curve();
    }
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
            //        std::cout << i << " " << j << " " << f_pic[i * data.rows() + j][0] << " " << f_pic[i * data.rows() + j][1]
            //                  << std::endl;
        }
    }

    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_f.csv");

    for (int i = 0; i < picture.rows(); ++i) {
        for (int j = 0; j < picture.cols(); ++j) {
            out2 << norm(res(i, j)) << ",";
        }
        out2 << std::endl;
    }

    fftw_destroy_plan(plan);
    fftw_cleanup();
    fftw_free(f_pic);
    fftw_free(pic_fftw);

    return res;
}

double DispersionCurve::calc_vcosalpha(const std::vector<double> &omega, const std::vector<double> &k_num,
                                       const std::vector<double> &sigma) {
    double k_num_sq_sum = 0.;
    double omega_sum = 0.;
    double k_num_sum = 0.;
    for (int i = 0; i < omega.size(); ++i) {
        k_num_sq_sum += std::sqrt(k_num[i]) / sigma[i];
        omega_sum += omega[i] / sigma[i];
        k_num_sum += k_num[i] / sigma[i];
    }
    return (omega_sum * M_PI * 2. - std::sqrt(G_COEFF) * k_num_sq_sum) / k_num_sum;
}

double DispersionCurve::dispersion_func(const double k_num, const double vcosalpha) {
    return (std::sqrt(G_COEFF * k_num) + k_num * vcosalpha) / M_PI / 2.; //
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

            // detrending
            Eigen::VectorXcd data_ij = Eigen::VectorXd::Zero(data_fourier.size());
            Eigen::VectorXcd x_ij = Eigen::VectorXd::Zero(data_ij.size());
            for (int t = 0; t < data_fourier.size(); ++t) {
                data_ij[t] = data_fourier[t](i, j);
                x_ij[t] = static_cast<std::complex<double>>(t);
            }
            data_ij -= Eigen::VectorXcd::Ones(data_ij.size()) * data_ij.mean();
            auto size = static_cast<double>(data_ij.size());
            std::complex<double> x_ij_sum = x_ij.sum();
            std::complex<double> d_ij_sum = data_ij.sum();
            std::complex<double> a_coeff =
                    (size * data_ij.dot(x_ij) - d_ij_sum * x_ij_sum) /
                    (size * x_ij.norm() * x_ij.norm() - x_ij_sum * x_ij_sum);

            std::complex<double> b_coeff = (d_ij_sum - a_coeff * x_ij_sum) / size;

            data_ij -= (a_coeff * x_ij + b_coeff * Eigen::VectorXcd::Ones(data_ij.size()));


            for (int t = 0; t < data_fourier.size(); ++t) {
                double hann = 0.5 * (1 - std::cos(2 * M_PI * t / static_cast<double>(data_fourier.size()))); // or 1
                inp[t][0] = hann * data_ij[t].real();
                inp[t][1] = hann * data_ij[t].imag();
            }
            fftw_execute(plan);

            for (int t = 0; t < data_fourier.size(); ++t) {
                res[t](i, j) = out[t][0] * out[t][0] + out[t][1] * out[t][1];
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

void DispersionCurve::calc_curve() {

    // mirroring
    int half_size = static_cast<int>(picture.rows()) / 2;
    Eigen::MatrixXd down = picture.block(0, 0, half_size, picture.cols());
    Eigen::MatrixXd up = picture.block(half_size, 0, half_size, picture.cols());

    picture.block(0, 0, half_size, picture.cols()) += up.colwise().reverse();
    picture.block(half_size, 0, half_size, picture.cols()) += down.colwise().reverse();

    // marking points of columns maximum
    Eigen::VectorXi max_freq = argumax(picture, 0); // vec of argumax by freq for each wave number
    Eigen::VectorXd grad = gradient(max_freq); // gradient of vec above

    /* for me: picture.rows = 256 (freq), picture.cols = 32 (wave_num) */

    int argmax_max_freq = argumax(max_freq);

    if ((max_freq.maxCoeff() > (0.5 * static_cast<double>(picture.rows())) - 10) &&
        (grad.segment(argmax_max_freq + 1, grad.size() - argmax_max_freq).sum() < 0)) {
        max_freq.segment(argmax_max_freq + 1, max_freq.size() - argmax_max_freq) *= -1;
        max_freq.segment(argmax_max_freq + 1, max_freq.size() - argmax_max_freq) +=
                picture.rows() * Eigen::VectorXi::Ones(max_freq.size() - argmax_max_freq);
    }

    Eigen::VectorX<bool> mask = Eigen::VectorX<bool>::Zero(grad.size());
    Eigen::VectorXd grad_sorted = abs(grad); // below used module of gradient!
    std::sort(grad_sorted.begin(), grad_sorted.end());
    double grad_med = grad_sorted[static_cast<int>(grad_sorted.size() / 2)]; // median

    for (int i = 0; i < mask.size(); ++i) {
        if (std::abs(grad[i]) < 3 * grad_med) mask[i] = true;
        if (max_freq[i] <= static_cast<double >(picture.rows()) / 10.) mask[i] = false;
        if (max_freq[i] >= 9. * static_cast<double >(picture.rows()) / 10) mask[i] = false;
    }
    max_freq[0] = 0;
    mask[0] = true;

    std::cout << "mask " << mask.transpose() << std::endl;

    std::vector<double> max_freq_masked; // y of fitting points
    std::vector<double> k_num_vec; // x of fitting points
    std::vector<double> sigma_vec; // inverse weights of fitting points

    for (int i = 0; i < mask.size(); ++i) {
        if (mask[i]) {
            max_freq_masked.push_back(max_freq[i] / static_cast<double>(picture.rows()) / TURN_PERIOD);
            k_num_vec.push_back(static_cast<double>(i) / static_cast<double>(mask.size()) * max_wave_num);
            sigma_vec.push_back(1. / picture.col(i).maxCoeff());
        }
    }

    double vcosalpha = 0.; // if everything is bad, we believe that we measure in drift

    if (static_cast<double>(max_freq_masked.size()) > static_cast<double>(picture.cols()) * 0.2) {
        vcosalpha = calc_vcosalpha(max_freq_masked, k_num_vec, sigma_vec); // fitted coefficient
    }

    std::cout << "vcos " << vcosalpha << std::endl;

    Eigen::MatrixXd noise = Eigen::MatrixXd(picture); // there we cut area around dispersion curve

    /*
    Eigen::VectorXi left_vec = Eigen::VectorXi::Zero(picture.cols());
    Eigen::VectorXi right_vec = Eigen::VectorXi::Zero(picture.cols());
     */

    for (int k = 0; k < picture.cols(); ++k) { // loop for columns

        // calculating frequency of current wave number
        double freq = dispersion_func(static_cast<double>(k) / static_cast<double>(picture.cols()) * max_wave_num,
                                      vcosalpha);
        freq *= (max_index * TURN_PERIOD);

        int left = std::max(static_cast<int>(std::round(freq - width)), 0); // left side of curve
        int right = std::min(static_cast<int>(std::round(freq + width)), max_index); // right side of curve
        if (right > left) {
            noise.col(k).segment(left, right - left).setZero(); // zeroing signal
        }

        /*
        left_vec[k] = left;
        right_vec[k] = right;
        */

        // analogically for mirroring part
        freq *= -1;
        freq += max_index - 1;

        left = std::max(static_cast<int>(std::round(freq - width)), 0);
        right = std::min(static_cast<int>(std::round(freq - width)), max_index);
        if (right > left) {
            noise.col(k).segment(left, right - left).setZero();
        }
    }

    Eigen::MatrixXd signal = picture - noise; // there only data around curve
    for (int k = 1; k < signal.cols(); ++k) {
        signal.col(k) *= std::pow(k, BETA); // magic operation from (Borge, 2008)
    }

    Eigen::VectorXd ss = trapezoid(signal); // convolution throw wave number for signal
    Eigen::VectorXd nn = trapezoid(noise); // convolution throw wave number for noise

    for (int i = 1; i < signal.rows(); ++i) { // calculating a spectrum
        spectrum_struct.freq_spec[i] = ss[i] / nn[i];
    }

    spectrum_struct.peak_period = TURN_PERIOD / (static_cast<double>(argumax(spectrum_struct.freq_spec)) /
                                                 static_cast<double >(spectrum_struct.freq_spec.size()));

    spectrum_struct.m0 = trapezoid(ss) / trapezoid(nn); // calculating zeroth momentum

    Eigen::VectorXd freq_vec = Eigen::VectorXd::Zero(ss.size());
    freq_vec.setLinSpaced(0, 1 / TURN_PERIOD);

    spectrum_struct.m1 = trapezoid(ss * freq_vec) / trapezoid(nn * freq_vec); // calculating first momentum

    /*
    std::ofstream out2("/home/leeiozh/ocean/seavisionCPP/test_curve.csv");
    for (int i = 0; i < picture.rows(); ++i) {
        for (int j = 0; j < picture.cols(); ++j) {
            out2 << picture(i, j) << ",";
        }
        out2 << std::endl;
    }

    std::ofstream out3("/home/leeiozh/ocean/seavisionCPP/left_mark.csv");
    for (int i: left_vec) {
        out3 << i << ",";
    }
    std::ofstream out4("/home/leeiozh/ocean/seavisionCPP/right_mark.csv");
    for (int i: right_vec) {
        out4 << i << ",";
    }
    */

}

SpectrumStruct DispersionCurve::get_params() {
    return spectrum_struct;
}

} // namespace