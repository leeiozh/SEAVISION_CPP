//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_STATES_HPP
#define POLYGON_STATES_HPP

#include <string>
#include <eigen3/Eigen/Dense>

namespace Polygon {

struct ScopeState {
    Eigen::Vector3d position; // положение телескопа в GCRS
    Eigen::Vector3d direction; // направление визирной оси телескопа в GCRS
};

struct SatState {
    int number; // порядковый номер спутника
    double radius; // эффективный радиус отражения как сферы
    Eigen::Vector3d position3d; // положение аппарата в GCRS
    Eigen::Vector2d position2d = Eigen::Vector2d::Zero(); // положение в плоскости изображений
    double bright = 0.; // яркость
};

struct Star {
    int area_number; // номер зоны, к которой причислена звезда
    int inarea_number; // номер звезды внутри зоны
    int instar_component; // номер звезды как компонента большей звезды
    double ascension; // прямое восхождение
    double declination; // склонение
    double magnitude; // видимая звездная величина
    double bright; // яркость звезды
    Eigen::Vector2d position2d; // положение звезды в плоскости изображений
};

struct ExterParams {
    double hwhm = 0; // радиус размытия точки (нормированный на выходную апертуру)
    double p_diego = 0; // коэффициент в формуле
    double pr_diego = 0; // коэффициент в формуле
    double b_moffat = 3; // коэффициент степени в формуле
};

struct OpticParams {
    double inp_diam; // входная апертура
    double out_diam; // выходная апертура
    double trans_coeff; // коэффициент пропускания
    int iso_number = 1; // число изопланатических зон
    Eigen::MatrixXd c_coeffs; // коэффициенты разложения функции аберрации по косинусам
    Eigen::MatrixXd s_coeffs; // коэффициенты разложения функции аберрации по синусам
    ExterParams exter_params; // параметры среды, влияющие на фрт
};

struct NoiseParams {
    int warm; // средний тепловой шум (электрон)
    int count; // средний шум считывания (электрон)
};

struct MatrixParams {
    double pix_x; // размер одного пиксела
    double pix_y; // размер одного пиксела
    double matr_x; // размер матрицы
    double matr_y; // размер матрицы
    NoiseParams noise_params; // параметры шума
    int pix_capacity; // зарядовая емкость пиксела
};


} // namespace Polygon

#endif //POLYGON_STATES_HPP
