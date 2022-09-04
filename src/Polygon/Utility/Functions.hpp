//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_FUNCTIONS_HPP
#define POLYGON_FUNCTIONS_HPP

#include <random>
#include <fftw3.h>

namespace Polygon {

inline double calc_jp(const double &angle) {
    return 0.33 * (std::cos(angle) + 1);
}

inline double normal_random(const NoiseParams &noiseParams) {
    int sum_noise = noiseParams.count + noiseParams.warm;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> dist(0, sum_noise);
    return dist(gen);
}

template <int SIZE_X, int SIZE_Y>

inline fftw_complex *convert_eigen_to_fftw(const Eigen::MatrixXd &matrix) {
    fftw_complex *res;
    res = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));
    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            res[SIZE_X * i + j][0] = matrix(i, j);
            res[SIZE_X * i + j][1] = 0.;
        }
    }
    return res;
}

template <int SIZE_X, int SIZE_Y>

inline Eigen::MatrixXd convert_fftw_to_eigen(const fftw_complex *matrix) {
    Eigen::MatrixXd res = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);
    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            double real = matrix[i * SIZE_X + j][0];
            double imag = matrix[i * SIZE_X + j][1];
            res(i, j) = std::sqrt(real * real + imag * imag);
        }
    }
    return res;
}

}

#endif //POLYGON_FUNCTIONS_HPP
