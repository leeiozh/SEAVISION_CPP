//
// Created by leeiozh on 16.04.23.
//

#include "DispersionCurve.hpp"
#include <fstream>

namespace SeaVision {

DispersionCurve::DispersionCurve() {
    data_fourier.resize(FOUR_NUM);
}

void DispersionCurve::update(const int &index, const Eigen::MatrixXd &data) {

    data_fourier[index % FOUR_NUM] = calc_fourier_2d_one(data);

    if (index >= FOUR_NUM) {
        auto welch = calc_welch(index);
        picture = calc_abs_wave_num(welch);
        picture /= picture.maxCoeff();
        calc_curve();
    }
}


Eigen::MatrixXcd DispersionCurve::calc_fourier_2d_one(const Eigen::MatrixXd &data) {

    if (data.rows() == 0 || data.cols() == 0) {
        std::stringstream buff;
        buff << "Empty array received for Fourier transform!!";
        throw SeaVisionException(buff.str().c_str());
    }

    fftw_complex *f_pic = (fftw_complex *) fftw_malloc(data.size() * sizeof(fftw_complex));
    fftw_complex *pic_fftw = (fftw_complex *) fftw_malloc(data.size() * sizeof(fftw_complex));

    fftw_plan plan = fftw_plan_dft_2d(static_cast<int>(data.rows()), static_cast<int>(data.cols()), pic_fftw, f_pic,
                                      FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            pic_fftw[data.cols() * i + j][0] = data(i, j);
            pic_fftw[data.cols() * i + j][1] = 0.;
        }
    }
    fftw_execute(plan);

    Eigen::MatrixXcd res = Eigen::MatrixXd::Zero(CUT_NUM, CUT_NUM);
    for (int i = 0; i < CUT_NUM; ++i) {
        for (int j = 0; j < CUT_NUM; ++j) {
            res(i, j) = std::complex<double>(f_pic[i * data.rows() + j][0], f_pic[i * data.rows() + j][1]);
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(f_pic);
    fftw_free(pic_fftw);

    return res;
}

double DispersionCurve::calc_vcos(const std::vector<VCO> &vco_vec, const int counter) {

    double up = 0.;
    double down = 0.;

    for (int i = 0; i < counter; ++i) {
        up += vco_vec[i].calc_up();
        down += vco_vec[i].calc_down();
    }

    /*for (int i = 0; i < omega.size(); ++i) {
        up_sum += k_num[i] / sigma[i] / sigma[i] * (2 * M_PI * omega[i] - std::sqrt(G_COEFF * k_num[i]));
        low_sum += k_num[i] * k_num[i] / sigma[i] / sigma[i];
    }*/

    return up / down;
}

double DispersionCurve::dispersion_func(const double k_num, const double vcosalpha) {
    return (std::sqrt(G_COEFF * k_num) + k_num * vcosalpha) / M_PI / 2.; //
}

Eigen::VectorX<Eigen::MatrixXd> DispersionCurve::calc_welch(const int &index) const {

    Eigen::VectorX<Eigen::MatrixXd> res = Eigen::VectorX<Eigen::MatrixXd>(FOUR_NUM);

    for (int t = 0; t < FOUR_NUM; ++t) {
        res[t] = Eigen::MatrixXd::Zero(data_fourier[0].rows(), data_fourier[0].cols());
    }

    for (int i = 0; i < data_fourier[0].rows(); ++i) {
        for (int j = 0; j < data_fourier[0].cols(); ++j) {

            fftw_complex *out = (fftw_complex *) fftw_malloc(FOUR_NUM * sizeof(fftw_complex));

            fftw_complex *inp = (fftw_complex *) fftw_malloc(FOUR_NUM * sizeof(fftw_complex));

            fftw_plan plan = fftw_plan_dft_1d(FOUR_NUM, inp, out, FFTW_FORWARD, FFTW_ESTIMATE);

            // detrending
            Eigen::VectorXcd data_ij = Eigen::VectorXcd::Zero(FOUR_NUM);
            Eigen::VectorXcd x_ij = Eigen::VectorXcd::Zero(FOUR_NUM);
            for (int t = 0; t < FOUR_NUM; ++t) {
                int ind = (t + index % FOUR_NUM) % FOUR_NUM; // indexes shift for fourier array
                data_ij[t] = data_fourier[ind](i, j);
                x_ij[t] = static_cast<std::complex<double>>(ind);
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

            for (int t = 0; t < FOUR_NUM; ++t) {
                int ind = (t + index % FOUR_NUM + 1) % FOUR_NUM; // indexes shift for fourier array
                double hann = 0.5 * (1 - std::cos(2 * M_PI * t / static_cast<double>(FOUR_NUM))); // or 1
                inp[t][0] = hann * data_ij[ind].real();
                inp[t][1] = hann * data_ij[ind].imag();
            }
            fftw_execute(plan);

            for (int t = 0; t < FOUR_NUM; ++t) {
                res[t](i, j) = out[t][0] * out[t][0] + out[t][1] * out[t][1]; // square norm
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

    auto signal_noise_fir = proc_one_curve(picture, 0);

    Eigen::VectorXd ss_fir = trapezoid(signal_noise_fir.first, 0, -1); // convolution throw wave number for signal
    Eigen::VectorXd nn_fir = trapezoid(signal_noise_fir.second, 0, -1); // convolution throw wave number for noise

    for (int i = 1; i < ss_fir.size(); ++i) { // calculating a spectrum
        if (i < 15) {
            ss_fir[i] = 0.; // bad thing
        }
        spectrum_struct.freq_spec[i] = ss_fir[i] / nn_fir[i];
    }

    int spec_freq_argmax = argumax(spectrum_struct.freq_spec);
    /*int spec_k_argmax = argumax(Eigen::VectorXd(signal_noise_fir.first.row(spec_freq_argmax)));
    double freq_true = std::sqrt(G_COEFF * spec_k_argmax / CUT_NUM * K_MAX) / 2. / M_PI;

    spectrum_struct.peak_period[0] = TURN_PERIOD / freq_true;*/

    spectrum_struct.peak_period[0] = TURN_PERIOD / (static_cast<double>(spec_freq_argmax) /
                                                    static_cast<double>(spectrum_struct.freq_spec.size()));

    spectrum_struct.m0[0] = trapezoid(ss_fir, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

    auto signal_noise_sec = proc_one_curve(signal_noise_fir.second, 1); // we try to identify second system

    Eigen::VectorXd ss_sec = trapezoid(signal_noise_sec.first, 0, -1); // convolution throw wave number for signal
    Eigen::VectorXd nn_sec = trapezoid(signal_noise_sec.second, 0, -1); // convolution throw wave number for noise
    Eigen::VectorXd freq_spec_sec = Eigen::VectorXd::Zero(FOUR_NUM);

    for (int i = 1; i < ss_sec.size(); ++i) { // calculating a spectrum
        if (i < 15) {
            ss_sec[i] = 0.; // bad thing
        }
        freq_spec_sec[i] = ss_sec[i] / nn_fir[i];
    }

    spec_freq_argmax = argumax(freq_spec_sec);
    /*spec_k_argmax = argumax(Eigen::VectorXd(signal_noise_sec.first.row(spec_freq_argmax)));
    freq_true = std::sqrt(G_COEFF * spec_k_argmax / CUT_NUM * K_MAX) / 2. / M_PI;*/

    //spectrum_struct.peak_period[1] = TURN_PERIOD / freq_true;
    spectrum_struct.peak_period[1] = TURN_PERIOD / (static_cast<double>(argumax(freq_spec_sec)) / FOUR_NUM);

    spectrum_struct.m0[1] = trapezoid(ss_sec, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

    auto signal_noise_th = proc_one_curve(signal_noise_sec.second, 2); // we try to identify second system

    Eigen::VectorXd ss_th = trapezoid(signal_noise_th.first, 0, -1); // convolution throw wave number for signal
    Eigen::VectorXd nn_th = trapezoid(signal_noise_th.second, 0, -1); // convolution throw wave number for noise
    Eigen::VectorXd freq_spec_th = Eigen::VectorXd::Zero(FOUR_NUM);

    for (int i = 1; i < ss_th.size(); ++i) { // calculating a spectrum
        if (i < 15) {
            ss_th[i] = 0.; // bad thing
        }
        freq_spec_th[i] = ss_th[i] / nn_fir[i];
    }
    spec_freq_argmax = argumax(freq_spec_th);
    /*spec_k_argmax = argumax(Eigen::VectorXd(signal_noise_th.first.row(spec_freq_argmax)));
    freq_true = std::sqrt(G_COEFF * spec_k_argmax /  CUT_NUM * K_MAX) / 2. / M_PI;

    spectrum_struct.peak_period[2] = TURN_PERIOD / freq_true;*/
    spectrum_struct.peak_period[2] = TURN_PERIOD / (static_cast<double>(argumax(freq_spec_th)) / FOUR_NUM);
    spectrum_struct.m0[2] = trapezoid(ss_th, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

}

std::pair<Eigen::MatrixXd, Eigen::MatrixXd>
DispersionCurve::proc_one_curve(const Eigen::MatrixXd &pic, const int &times) {

    std::ofstream out("C:/ocean/SEAVISION_CPP/curve" + std::to_string(times) + ".csv");
    for (int i = 0; i < pic.rows(); ++i) {
        for (int j = 0; j < pic.cols(); ++j) {
            out << pic(i, j) << ",";
        }
        out << std::endl;
    }

    // marking points of columns maximum
    Eigen::VectorXi max_freq = argumax(pic, 0); // vec of argumax by freq for each wave number
    Eigen::VectorXd grad = gradient_fir(max_freq); // gradient of vec above

    /* for me: picture.rows = 256 (freq), picture.cols = 32 (wave_num) */

    int argmax_max_freq = argumax(max_freq);

    if ((max_freq.maxCoeff() > (0.5 * static_cast<double>(pic.rows())) - 10) &&
        (grad.segment(argmax_max_freq + 1, grad.size() - argmax_max_freq - 1).sum() < 0)) {
        max_freq.segment(argmax_max_freq + 1, max_freq.size() - argmax_max_freq - 1) *= -1;
        max_freq.segment(argmax_max_freq + 1, max_freq.size() - argmax_max_freq - 1) +=
                pic.rows() * Eigen::VectorXi::Ones(max_freq.size() - argmax_max_freq - 1);
    }

    Eigen::VectorX<bool> mask = Eigen::VectorX<bool>::Zero(grad.size());
    Eigen::VectorXd grad_sorted = abs(grad); // below used module of gradient!
    std::sort(grad_sorted.begin(), grad_sorted.end());
    double grad_med = grad_sorted[static_cast<int>(grad_sorted.size() / 2)]; // median

    for (int i = 0; i < mask.size(); ++i) {
        if (std::abs(grad[i]) < 3 * grad_med) mask[i] = true;
        if (max_freq[i] <= static_cast<double >(pic.rows()) / 10.) mask[i] = false;
        if (max_freq[i] >= 9. * static_cast<double >(pic.rows()) / 10.) mask[i] = false;
    }

    max_freq[0] = 0;    // forced zero crossing
    mask[0] = true;     // forced zero crossing

    std::vector<VCO> vco_vec(CUT_NUM);   // structure of fitting points
    int counter = 0;

    for (int i = 0; i < mask.size(); ++i) { // filling arrays for fitting
        if (mask[i]) {
            vco_vec[counter].omega = max_freq[i] / static_cast<double>(pic.rows()) / TURN_PERIOD;
            vco_vec[counter].k_num = static_cast<double>(i) / static_cast<double>(mask.size()) * K_MAX;
            vco_vec[counter].sigma = pic.col(i).maxCoeff();
            counter++;
        }
    }

    double vcosalpha = 0.; // if everything is bad, we suppose that we measure in drift

    if (counter > static_cast<double>(pic.cols()) * 0.2) {
        vcosalpha = calc_vcos(vco_vec, counter); // fitted coefficient
    }

    spectrum_struct.vcosalpha[times] = vcosalpha;

    Eigen::MatrixXd noise = Eigen::MatrixXd(pic); // there we cut area around dispersion curve

    for (int k = 0; k < pic.cols(); ++k) { // loop for columns

        // calculating frequency of current wave number
        double freq = dispersion_func(static_cast<double>(k) / CUT_NUM * K_MAX, vcosalpha);
        freq *= (FOUR_NUM * TURN_PERIOD);

        int left = std::max(static_cast<int>(std::round(freq - DELTA_FREQ)), 0); // left side of curve
        int right = std::min(static_cast<int>(std::round(freq + DELTA_FREQ)), FOUR_NUM); // right side of curve
        if (right > left) noise.col(k).segment(left, right - left).setZero(); // zeroing signal
    }

    Eigen::MatrixXd signal = pic - noise; // there only data around curve

    for (int k = 0; k < pic.cols(); ++k) { // loop for columns

        // calculating frequency of current wave number
        double freq = dispersion_func(static_cast<double>(k) / CUT_NUM * K_MAX, vcosalpha);
        freq *= (FOUR_NUM * TURN_PERIOD);

        // analogically for mirroring part
        freq *= -1;
        freq += FOUR_NUM - 1;

        int left = std::max(static_cast<int>(std::round(freq - DELTA_FREQ + 1)), 0);
        int right = std::min(static_cast<int>(std::round(freq + DELTA_FREQ + 1)), FOUR_NUM);
        if (right > left) {
            noise.col(k).segment(left, right - left).setZero();
        }
    }

    for (int k = 1; k < signal.cols(); ++k) {
        signal.col(k) *= std::pow(k, BETA); // magic operation from (Borge, 2008)
    }

    return std::make_pair(signal, noise);
}

WaveStruct DispersionCurve::get_params() {
    return spectrum_struct;
}

} // namespace