//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_FUNCTIONS_HPP
#define POLYGON_FUNCTIONS_HPP

#include <random>
#include <fftw3.h>

namespace Polygon {

/**
 * функция JP(xi)
 * @param angle угол раствора конуса обзора
 * @return функция диффузного отражения как от сферы
 */
inline double calc_jp(const double &angle) {
    return 0.33 * (std::cos(angle) + 1);
}

/**
 * функция, отдающее случайное число, распределенное нормально
 * @param noiseParams параметры шума
 * @return рандомное число электрон по нормальному распределению
 */
inline double normal_random(const NoiseParams &noiseParams) {
    int sum_noise = noiseParams.count + noiseParams.warm;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> dist(0, sum_noise);
    return dist(gen);
}

template <int SIZE_X, int SIZE_Y>

/**
 * функция конвертации матрицы Eigen::MatrixXd в массив чисел типа, с которым работает FFTW
 * @tparam SIZE_X размер матрицы
 * @tparam SIZE_Y размер матрицы
 * @param matrix структура с параметрами матрицы
 * @param norm нужно ли нормировать значения матрицы
 * @return указатель на массив с числами типа fftw_complex
 */
inline fftw_complex *convert_eigen_to_fftw(const Eigen::MatrixXd &matrix, const bool &norm = false) {
    fftw_complex *res;
    res = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));
    double max = 1.;
    if (norm) max = matrix.maxCoeff();

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            res[SIZE_X * i + j][0] = matrix(i, j) / max;
            res[SIZE_X * i + j][1] = 0.;
        }
    }
    return res;
}

template <int SIZE_X, int SIZE_Y>
/**
 * функция конвертации массива чисел типа, с которым работает FFTW, в матрицу типа Eigen::MatrixXd
 * @tparam SIZE_X размер матрицы
 * @tparam SIZE_Y размер матрицы
 * @param matrix структура с параметрами матрицы
 * @param norm нужно ли нормировать значения матрицы
 * @return матрицу Eigen::MatrixXd
 */
inline Eigen::MatrixXd convert_fftw_to_eigen(const fftw_complex *matrix, const bool &norm = false) {
    Eigen::MatrixXd res = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            res(i, j) = matrix[i * SIZE_X + j][0];
        }
    }
    double max = 1.;
    if (norm) max = res.maxCoeff();
    return res / max;
}

}

#endif //POLYGON_FUNCTIONS_HPP
