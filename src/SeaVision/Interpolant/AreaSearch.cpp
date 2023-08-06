//
// Created by leeiozh on 6/21/23.
//

#include <fstream>
#include <iostream>
#include "AreaSearch.hpp"

namespace SeaVision {

AreaSearch::AreaSearch(const int az_zone) : az_zone(az_zone) {
    curr_az = 0.;
    curr_az_ind = 0;
    rose.resize(az_zone);
    curr_std = 0.;
};


int AreaSearch::search_area(const std::vector<Eigen::MatrixXi> &data) {

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

    /* std::ofstream out3("/home/leeiozh/ocean/seavisionCPP/test_std.csv");

    for (int i = 0; i < std_back.rows(); ++i) {
        for (int j = 0; j < std_back.cols(); ++j) {
            out3 << std_back(i, j) << ",";
        }
        out3 << std::endl;
    } */

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

int AreaSearch::search_dir(const Eigen::MatrixXi &data) {

    int height = std::ceil(static_cast<double>(data.cols()) / NUM_AREA);
    Eigen::VectorXd disp = Eigen::VectorXd::Zero(NUM_AREA);

    for (int i = 0; i < NUM_AREA; ++i) {
        int start_col = i * height - static_cast<int>(height / 4.);
        int block_cols = static_cast<int>(6. * static_cast<double>(height) / 4.);
        if (i == 0) start_col = 0;
        if ((i + 1) == NUM_AREA) block_cols = static_cast<int>(5. * static_cast<double>(height) / 4.);

        Eigen::MatrixXi trim = data.block(360, start_col, 360, block_cols);

        Eigen::VectorXd sum = Eigen::VectorXd::Zero(trim.rows());

        for (int j = 0; j < trim.rows(); ++j) {
            sum[j] = trim.row(j).sum();
        }

        disp[i] = (sum - sum.mean() * Eigen::VectorXd::Ones(sum.size())).norm() / static_cast<double>(trim.size());
    }

    rose = disp;

    curr_std = argumax(disp);
    return curr_std;
}

Eigen::VectorXd AreaSearch::get_rose() const {
    return rose;
}

int AreaSearch::get_curr_az_ind() const {
    return curr_az_ind;
}

int AreaSearch::get_curr_std() const {
    return curr_std;
}


} // namespace
