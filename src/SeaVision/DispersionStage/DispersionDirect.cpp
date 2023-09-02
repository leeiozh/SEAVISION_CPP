//
// Created by leeiozh on 6/21/23.
//

#include "DispersionDirect.hpp"

namespace SeaVision {

DispersionDirect::DispersionDirect(const int az_zone) : az_zone(az_zone) {}

int DispersionDirect::search_area(const std::vector<Eigen::MatrixXi> &data) {

    Eigen::MatrixXd std_back = Eigen::MatrixXd::Zero(data[0].rows(), data[0].cols());

    for (int i = 0; i < data[0].rows(); ++i) { // distance
        for (int j = 0; j < data[0].cols(); ++j) { // azimuth

            Eigen::VectorXd line = Eigen::VectorXd::Zero(static_cast<int>(data.size()));
            for (int t = 0; t < data.size(); ++t) {
                line[t] = data[t](i, j);
            }

            Eigen::VectorXd mean = line.mean() * Eigen::VectorXd::Ones(line.size());
            std_back(i, j) = (line - mean).norm(); // it's not standard deviation because we don't divide on size
        }
    }

    Eigen::VectorXd std_smooth = Eigen::VectorXd::Zero(az_zone);

    for (int n = 0; n < az_zone; ++n) {
        int size_one_zone = static_cast<int>(data[0].cols() / az_zone);
        std_smooth[n] = std_back.block(0, size_one_zone * n, data[0].rows(), size_one_zone).mean();
    }

    int max_ind = 0.;
    double max_el = std_smooth.maxCoeff();
    for (int i = 0; i < std_smooth.size(); ++i) {
        if (std_smooth[i] == max_el) {
            max_ind = i;
            break;
        }
    }

    curr_az_ind = max_ind;
    double res = static_cast<double >(max_ind) / static_cast<double >(std_smooth.size()) * 360.;

    // if current angle changed significantly then go to it by steps

    if (std::abs(res - curr_az) > 720. / az_zone) {
        ((res - curr_az) > 0) ? curr_az += 360. / az_zone : curr_az -= 360. / az_zone;
        ((res - curr_az) > 0) ? curr_az_ind += 1 : curr_az_ind -= 1;
        if (curr_az_ind < 0) curr_az_ind += az_zone;
        if (curr_az_ind >= az_zone) curr_az_ind -= az_zone;
        if (curr_az < 0) curr_az += 360.;
        if (curr_az >= 360.) curr_az -= 360.;
    } else {
        curr_az = res;
    }

    return curr_az_ind;
}

void DispersionDirect::smooth_change(const Eigen::VectorXi &new_vec) {

    for (int i = 0; i < new_vec.size(); ++i) {
        if (std::abs(new_vec[i] - curr_std[i]) > 2) {
            ((new_vec[i] - curr_std[i]) > 0) ? curr_std[i] += 1 : curr_std[i] -= 1;
            if (curr_std[i] < 0) curr_std[i] += az_zone;
            if (curr_std[i] >= az_zone) curr_std[i] -= az_zone;
        } else {
            curr_std[i] = new_vec[i];
        }
    }
}

Eigen::VectorXi DispersionDirect::calc_directions(const Eigen::MatrixXi &data, const int index, const double step) {

    int height = std::ceil(static_cast<double>(data.cols()) / NUM_AREA); // height of rectangle for summing
    Eigen::VectorXd disp = Eigen::VectorXd::Zero(NUM_AREA); // vector of dispersion in each rectangle

    for (int i = 0; i < NUM_AREA; ++i) { // loop by rectangles
        int start_col = i * height - static_cast<int>(height / 4.); // rectangles overlap each other by a
        int block_cols = static_cast<int>(6. * static_cast<double>(height) / 4.); // quarter of the height
        if (i == 0) start_col = 0;
        if ((i + 1) == NUM_AREA) block_cols = static_cast<int>(5. * static_cast<double>(height) / 4.);

        // trimmed a rectangle
        Eigen::MatrixXi trim = data.block(360, start_col, 360, block_cols);
        Eigen::VectorXd sum = Eigen::VectorXd::Zero(trim.rows()); // integral throw angles on one distance

        for (int j = 0; j < trim.rows(); ++j) {
            sum[j] = trim.row(j).sum(); // summing throw vertical
        }

        // dispersion of this sum
        disp[i] = (sum - sum.mean() * Eigen::VectorXd::Ones(sum.size())).norm() / static_cast<double>(trim.size());
    }

    Eigen::VectorXi new_std = argumax(disp, NUM_SYSTEMS, 2);

    if (index > MEAN) {
        smooth_change(new_std);
    } else {
        curr_std = new_std;
    }

    rose = disp;

    calc_length(data, step);

    return curr_std;
}

Eigen::VectorXd DispersionDirect::calc_length(const Eigen::MatrixXi &data, const double step) {

    int height = std::ceil(static_cast<double>(data.cols()) / NUM_AREA);

    for (int n = 0; n < NUM_SYSTEMS; ++n) {

        int i = curr_std[n];

        int start_col = i * height - static_cast<int>(height / 4.); // rectangles overlap each other by a
        int block_cols = static_cast<int>(6. * static_cast<double>(height) / 4.); // quarter of the height
        if (i == 0) start_col = 0;
        if ((i + 1) == NUM_AREA) block_cols = static_cast<int>(5. * static_cast<double>(height) / 4.);

        // trimmed a rectangle
        Eigen::MatrixXi trim = data.block(360, start_col, 360, block_cols);
        Eigen::VectorXd sum = Eigen::VectorXd::Zero(trim.rows()); // integral throw angles on one distance

        for (int j = 0; j < trim.rows(); ++j) {
            sum[j] = trim.row(j).sum(); // summing throw vertical
        }

        auto *out = (fftw_complex *) fftw_malloc(sum.size() * sizeof(fftw_complex));
        auto *inp = (fftw_complex *) fftw_malloc(sum.size() * sizeof(fftw_complex));

        fftw_plan plan = fftw_plan_dft_1d(static_cast<int>(sum.size()), inp, out, FFTW_FORWARD, FFTW_ESTIMATE);

        for (int t = 0; t < sum.size(); ++t) {
            inp[t][0] = sum[t];
            inp[t][1] = 0.;
        }

        fftw_execute(plan);

        int half_size = std::floor(sum.size() / 2);

        Eigen::VectorXd spec = Eigen::VectorXd::Zero(half_size);

        for (int t = 4; t < half_size; ++t) {
            spec[t] = out[t][0] * out[t][0] + out[t][1] * out[t][1]; // square norm
        }

        fftw_destroy_plan(plan);
        fftw_free(inp);
        fftw_free(out);

        curr_len[n] = step * static_cast<double>(spec.size()) / static_cast<double>(argumax(spec));
    }

    return curr_len;
}

Eigen::VectorXd DispersionDirect::get_rose() const {
    Eigen::VectorXd rose_height = Eigen::VectorXd::Zero(rose.size());
    for (int i = 0; i < rose.size(); ++i) {
        rose_height[i] = C_COEFF + D_COEFF * rose[i];
    }
    return rose_height;
}

int DispersionDirect::get_curr_az_ind() const {
    return curr_az_ind;
}

Eigen::VectorXi DispersionDirect::get_curr_std() const {
    return curr_std;
}

Eigen::VectorXd DispersionDirect::get_len() const {
    return curr_len;
}

Eigen::VectorXd DispersionDirect::get_dirs() const {

    Eigen::VectorXd res = Eigen::VectorXd::Zero(NUM_SYSTEMS);

    for (int i = 0; i < NUM_SYSTEMS; ++i) {
        res[i] = static_cast<double>(curr_std[i]) / NUM_AREA * 360.;
    }

    return res;
}

Eigen::VectorXd DispersionDirect::get_swh() const {
    Eigen::VectorXd res = Eigen::VectorXd::Zero(NUM_SYSTEMS);

    for (int i = 0; i < NUM_SYSTEMS; ++i) {
        res[i] = C_COEFF + D_COEFF * rose[curr_std[i]];
    }

    return res;
}

} // namespace
