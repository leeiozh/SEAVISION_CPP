//
// Created by leeiozh on 16.04.23.
//

#include "DispersionCurve.hpp"


namespace SeaVision {

DispersionCurve::DispersionCurve(int max_index, int width, int cut_index, double max_wave_num) :
        max_index(max_index), width(width), cut_index(cut_index), max_wave_num(max_wave_num) {
    data_fourier.resize(max_index);
}

void DispersionCurve::update(const int index, const Eigen::MatrixXd &data) {

    data_fourier[index % max_index] = calc_fourier_2d_one(data);

    if (index >= max_index) {
        auto welch = calc_welch(index);
        picture = calc_abs_wave_num(welch);
        picture /= picture.maxCoeff();
        calc_curve();
    }
}


Eigen::MatrixXcd DispersionCurve::calc_fourier_2d_one(const Eigen::MatrixXd &data) const {

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

    Eigen::MatrixXcd res = Eigen::MatrixXd::Zero(cut_index, cut_index);
    for (int i = 0; i < cut_index; ++i) {
        for (int j = 0; j < cut_index; ++j) {
            res(i, j) = std::complex<double>(f_pic[i * data.rows() + j][0], f_pic[i * data.rows() + j][1]);
        }
    }

    fftw_destroy_plan(plan);
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

Eigen::VectorX<Eigen::MatrixXd> DispersionCurve::calc_welch(int index) const {

    Eigen::VectorX<Eigen::MatrixXd> res = Eigen::VectorX<Eigen::MatrixXd>(max_index);

    for (int t = 0; t < max_index; ++t) {
        res[t] = Eigen::MatrixXd::Zero(data_fourier[0].rows(), data_fourier[0].cols());
    }

    for (int i = 0; i < data_fourier[0].rows(); ++i) {
        for (int j = 0; j < data_fourier[0].cols(); ++j) {

            fftw_complex *out = (fftw_complex *) fftw_malloc(max_index * sizeof(fftw_complex));

            fftw_complex *inp = (fftw_complex *) fftw_malloc(max_index * sizeof(fftw_complex));

            fftw_plan plan = fftw_plan_dft_1d(max_index, inp, out, FFTW_FORWARD, FFTW_ESTIMATE);

            // detrending
            Eigen::VectorXcd data_ij = Eigen::VectorXcd::Zero(max_index);
            Eigen::VectorXcd x_ij = Eigen::VectorXcd::Zero(max_index);
            for (int t = 0; t < max_index; ++t) {
                int ind = (t + index % max_index) % max_index; // indexes shift for fourier array
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

            for (int t = 0; t < max_index; ++t) {
                int ind = (t + index % FOUR_NUM + 1) % max_index; // indexes shift for fourier array
                double hann = 0.5 * (1 - std::cos(2 * M_PI * t / static_cast<double>(max_index))); // or 1
                inp[t][0] = hann * data_ij[ind].real();
                inp[t][1] = hann * data_ij[ind].imag();
            }
            fftw_execute(plan);

            for (int t = 0; t < max_index; ++t) {
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

    spectrum_struct.peak_period[0] = TURN_PERIOD / (static_cast<double>(argumax(spectrum_struct.freq_spec)) /
                                                    static_cast<double>(spectrum_struct.freq_spec.size()));

    spectrum_struct.m0[0] = trapezoid(ss_fir, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

    auto signal_noise_sec = proc_one_curve(signal_noise_fir.second, 1); // we try to identify second system

    Eigen::VectorXd ss_sec = trapezoid(signal_noise_sec.first, 0, -1); // convolution throw wave number for signal
    Eigen::VectorXd nn_sec = trapezoid(signal_noise_sec.second, 0, -1); // convolution throw wave number for noise
    Eigen::VectorXd freq_spec_sec = Eigen::VectorXd::Zero(max_index);

    for (int i = 1; i < ss_sec.size(); ++i) { // calculating a spectrum
        if (i < 15) {
            ss_sec[i] = 0.; // bad thing
        }
        freq_spec_sec[i] = ss_sec[i] / nn_fir[i];
    }

    spectrum_struct.peak_period[1] = TURN_PERIOD / (static_cast<double>(argumax(freq_spec_sec)) / max_index);
    spectrum_struct.m0[1] = trapezoid(ss_sec, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

    auto signal_noise_th = proc_one_curve(signal_noise_sec.second, 2); // we try to identify second system

    Eigen::VectorXd ss_th = trapezoid(signal_noise_th.first, 0, -1); // convolution throw wave number for signal
    Eigen::VectorXd nn_th = trapezoid(signal_noise_th.second, 0, -1); // convolution throw wave number for noise
    Eigen::VectorXd freq_spec_th = Eigen::VectorXd::Zero(max_index);

    for (int i = 1; i < ss_th.size(); ++i) { // calculating a spectrum
        if (i < 15) {
            ss_th[i] = 0.; // bad thing
        }
        freq_spec_th[i] = ss_th[i] / nn_fir[i];
    }

    spectrum_struct.peak_period[2] = TURN_PERIOD / (static_cast<double>(argumax(freq_spec_th)) / max_index);
    spectrum_struct.m0[2] = trapezoid(ss_th, 10, -1) / trapezoid(nn_fir, 10, -1); // calculating zeroth momentum

}

std::pair<Eigen::MatrixXd, Eigen::MatrixXd>
DispersionCurve::proc_one_curve(const Eigen::MatrixXd &pic, int times) {

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
        if (max_freq[i] >= 9. * static_cast<double >(pic.rows()) / 10) mask[i] = false;
    }
    max_freq[0] = 0; // forced zero crossing
    mask[0] = true; // forced zero crossing

    //std::cout << "mask " << mask.transpose() << std::endl;

    std::vector<double> max_freq_masked; // y of fitting points
    std::vector<double> k_num_vec; // x of fitting points
    std::vector<double> sigma_vec; // inverse weights of fitting points

    for (int i = 0; i < mask.size(); ++i) { // filling arrays for fitting
        if (mask[i]) {
            max_freq_masked.push_back(max_freq[i] / static_cast<double>(pic.rows()) / TURN_PERIOD);
            k_num_vec.push_back(static_cast<double>(i) / static_cast<double>(mask.size()) * max_wave_num);
            sigma_vec.push_back(1. / pic.col(i).maxCoeff());
        }
    }

    double vcosalpha = 0.; // if everything is bad, we suppose that we measure in drift

    if (static_cast<double>(max_freq_masked.size()) > static_cast<double>(pic.cols()) * 0.2) {
        vcosalpha = calc_vcosalpha(max_freq_masked, k_num_vec, sigma_vec); // fitted coefficient
    }

    spectrum_struct.vcosalpha[times] = vcosalpha;

    //std::cout << "vcos " << vcosalpha << std::endl;
    Eigen::MatrixXd noise = Eigen::MatrixXd(pic); // there we cut area around dispersion curve

    for (int k = 0; k < pic.cols(); ++k) { // loop for columns

        // calculating frequency of current wave number
        double freq = dispersion_func(static_cast<double>(k) / cut_index * max_wave_num, vcosalpha);
        freq *= (max_index * TURN_PERIOD);

        int left = std::max(static_cast<int>(std::round(freq - width)), 0); // left side of curve
        int right = std::min(static_cast<int>(std::round(freq + width)), max_index); // right side of curve
        if (right > left) noise.col(k).segment(left, right - left).setZero(); // zeroing signal
    }

    Eigen::MatrixXd signal = pic - noise; // there only data around curve

    for (int k = 0; k < pic.cols(); ++k) { // loop for columns

        // calculating frequency of current wave number
        double freq = dispersion_func(static_cast<double>(k) / cut_index * max_wave_num, vcosalpha);
        freq *= (max_index * TURN_PERIOD);

        // analogically for mirroring part
        freq *= -1;
        freq += max_index - 1;

        int left = std::max(static_cast<int>(std::round(freq - width + 1)), 0);
        int right = std::min(static_cast<int>(std::round(freq + width + 1)), max_index);
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