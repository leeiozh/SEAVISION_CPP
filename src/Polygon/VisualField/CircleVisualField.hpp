//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_CIRCLEVISUALFIELD_HPP
#define POLYGON_CIRCLEVISUALFIELD_HPP

#include <utility>
#include <fstream>
#include "BaseVisualField.hpp"

namespace Polygon {

class CircleVisualField : public BaseVisualField {
    /**
     * класс поля зрения в форме круга, по углу обзора, направлению оси и положениям вычисляет, какие КО и звезды видит
     * телескоп и какие координаты имеют эти объекты в плоскости предметов, их яркость
     */

public:

    /**
     * конструктор класса
     * @param cone_angle угол раствора конуса обзора
     * @param view_sizes размеры передней апертуры
     * @throw PolygonBaseException в случае, если угол обзора не лежит в [0, pi/2]
     */
    inline explicit CircleVisualField(const double &cone_angle, Eigen::Vector2d view_sizes) : BaseVisualField(
            cone_angle, std::move(view_sizes)) {};

    /**
     * конструктор класса
     * @param cone_angle угол раствора конуса обзора
     * @param view_sizes размеры передней апертуры
     * @param path путь к папке с каталогом
     * @throw PolygonBaseException в случае, если угол обзора не лежит в [0, pi/2]
     */
    inline CircleVisualField(const double &cone_angle, Eigen::Vector2d view_sizes, const std::string &path)
            : BaseVisualField(cone_angle, std::move(view_sizes), path) {};

    /**
     * проверяет, попадает ли КО в конус обзора
     * @param scope_state структура с данными о положении и ориентации телескопа
     * @param sat_pos положение КО
     * @return попадает / не попадает
     */
    [[nodiscard]] bool check_sat(const ScopeState &scope_state,
                                 const Eigen::Vector3d &sat_pos) const override;

    /**
     * находит массив КО, входящих в конус обзора, параллельно вычисляя их координаты в плоскости предметов и яркость
     * @param scope_state структура с данными о положении и ориентации телескопа
     * @param sat_state вектор структур с данными о КО
     * @param sun_pos положение Солнца (необходимо для вычисления яркости)
     * @return массив структур с данными о КО
     */
    [[nodiscard]] std::vector<SatState> check_sat_array(const ScopeState &scope_state,
                                                        std::vector<SatState> &sat_state,
                                                        const Eigen::Vector3d &sun_pos) override;

    /**
     * находит массив номеров зон (внутренняя сетка каталога), которые задеваются конусом обзора
     * @param scope_dir направление оси телескопа
     * @return массив номеров зон
     */
    [[nodiscard]] std::vector<int> view_area_nums(const Eigen::Vector3d &scope_dir) const override;

    /**
     * находит массив звезд, входящих в конус обзора, параллельно вычисляя их положение в плоскости предметов и яркость
     * @param scope_dir направление оси телескопа
     * @return массив структур с данными о звездах
     */
    [[nodiscard]] std::vector<Star>
    view_star_array(const Eigen::Vector3d &scope_dir) const override;
};

} // namespace Polygon


#endif //POLYGON_CIRCLEVISUALFIELD_HPP
