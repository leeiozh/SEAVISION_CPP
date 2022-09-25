//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_MATRIX_HPP
#define POLYGON_MATRIX_HPP

#include "../Utility/Structs.hpp"
#include "../Utility/Consts.hpp"
#include "../Utility/Functions.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class Matrix {
    /**
     * класс матрицы
     */
protected:
    MatrixParams params; // параметры матрицы
    std::array<double, COLOR_NUMBER> light_length; // массив используемых длин волн
    std::array<double, COLOR_NUMBER> quant_efficiency; // массив коэффициентов квантовой эффективности на используемых длинах волн
    std::array<Eigen::MatrixXd, COLOR_NUMBER> pictures; // полученные изображения
public:

    /**
     * конструктор матрицы
     * @param matrixParams параметры матрицы
     * @param light_length массив используемых длин волн
     * @param quant_efficiency массив коэффициентов квантовой эффективности на используемых длинах волн
     */
    inline Matrix(const MatrixParams &matrixParams,
                  const std::array<double, COLOR_NUMBER> &light_length,
                  const std::array<double, COLOR_NUMBER> &quant_efficiency) : params(matrixParams),
                                                                              light_length(light_length),
                                                                              quant_efficiency(quant_efficiency) {}

    /**
     * конвертация интенсивности в количество фотоэлектрон без учета шума
     * @param time_expose время экспозиции
     * @return массив изображений
     */
    inline std::array<Eigen::MatrixXi, COLOR_NUMBER> to_electrons(const double &time_expose) {
        std::array<Eigen::MatrixXi, COLOR_NUMBER> res;
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            res[pic] = Eigen::MatrixXi::Zero(SIZE_X, SIZE_Y);
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    double elec = pictures[pic](i, j) * time_expose * params.pix_x * params.pix_y * light_length[pic] /
                                  PLUNK_CONSTANT_LIGHT_SPEED * 1e17; //1е17 - околоподгон, надо пересчитать
                    elec *= quant_efficiency[pic];
                    elec = std::min(static_cast<int>(elec), params.pix_capacity);
                    elec = std::max(elec, 0.);
                    res[pic](i, j) = static_cast<int>(elec);
                }
            }
        }
        return res;
    }

    /**
     * конвертация интенсивности в количество фотоэлектрон с учетом шума
     * @param time_expose время экспозиции
     * @return массив изображений
     */
    inline std::array<Eigen::MatrixXi, COLOR_NUMBER> to_electrons_with_noise(const double &time_expose) {

        std::array<Eigen::MatrixXi, COLOR_NUMBER> res;
        for (int pic = 0; pic < COLOR_NUMBER; ++pic) {
            res[pic] = Eigen::MatrixXi::Zero(SIZE_X, SIZE_Y);
            for (int i = 0; i < SIZE_X; ++i) {
                for (int j = 0; j < SIZE_Y; ++j) {
                    double elec = pictures[pic](i, j) * time_expose * params.pix_x * params.pix_y * light_length[pic] /
                                  PLUNK_CONSTANT_LIGHT_SPEED * 1e17; //1е17 - TODO: околоподгон, надо пересчитать

                    elec *= quant_efficiency[pic];
                    elec += normal_random(params.noise_params);
                    elec = std::min(static_cast<int>(elec), params.pix_capacity);
                    elec = std::max(elec, 0.);

                    res[pic](i, j) = static_cast<int>(elec);
                }
            }
        }

        return res;
    }

    // перегрузка оператора вызова
    inline double operator()(int i, int j, int k) {
        return pictures[i](j, k);
    }

    // геттер параметров матрицы
    inline MatrixParams get_params() {
        return params;
    }

    // сеттер матриц изображений
    // TODO: ЭТО СТРАННОЕ РЕШЕНИЕ, НАДО ПЕРЕСТРОИТЬ
    inline void set_pictures(const std::array<Eigen::MatrixXd, COLOR_NUMBER> &pics) {
        pictures = pics;
    }

};
}

#endif //POLYGON_MATRIX_HPP
