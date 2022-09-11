//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_BASEVISUALFIELD_HPP
#define POLYGON_BASEVISUALFIELD_HPP

#include <utility>

#include "../Utility/Structs.hpp"
#include "../Utility/Consts.hpp"
#include "../Utility/Functions.hpp"
#include "../PoligonBaseException.hpp"

namespace Polygon {

class BaseVisualField {
    /**
     * класс поля зрения, по углу обзора, направлению оси и положениям вычисляет, какие КО и звезды видит телескоп
     * и какие координаты имеют эти объекты в плоскости предметов, их яркость
     */
protected:
    double cone_angle; // угол раствора конуса обзора
    Eigen::Vector2d view_sizes; // размеры передней апертуры
    std::string path_to_res = "resources"; // путь к папке с каталогом

public:

    /**
     * конструктор класса
     * @param cone_angle угол раствора конуса обзора
     * @param view_sizes размеры передней апертуры
     * @throw PolygonBaseException в случае, если угол обзора не лежит в [0, pi/2]
     */
    inline BaseVisualField(const double &cone_angle, Eigen::Vector2d view_sizes) : cone_angle(
            cone_angle), view_sizes(std::move(view_sizes)) {
        if (cone_angle > M_PI_2 || cone_angle < 0) {
            std::stringstream buff;
            buff << "Угол раствора конуса обзора может принимать значения из [0, pi/2]! Текущий угол: "
                 << cone_angle << " радиан!" << std::endl;
            throw PolygonBaseException(buff.str().c_str());
        }
    };

    /**
     * конструктор класса
     * @param cone_angle угол раствора конуса обзора
     * @param view_sizes размеры передней апертуры
     * @param path путь к папке с каталогом
     * @throw PolygonBaseException в случае, если угол обзора не лежит в [0, pi/2]
     */
    inline BaseVisualField(const double &cone_angle, Eigen::Vector2d view_sizes, std::string path) :
            path_to_res(std::move(path)), cone_angle(cone_angle), view_sizes(std::move(view_sizes)) {};

    /**
     * проверяет, попадает ли КО в конус обзора
     * @param scope_state структура с данными о положении и ориентации телескопа
     * @param sat_pos положение КО
     * @return попадает / не попадает
     */
    [[nodiscard]] inline virtual bool
    check_sat(const ScopeState &scope_state,
              const Eigen::Vector3d &sat_pos) const = 0;

    /**
     * находит массив КО, входящих в конус обзора, параллельно вычисляя их координаты в плоскости предметов и яркость
     * @param scope_state структура с данными о положении и ориентации телескопа
     * @param sat_state вектор структур с данными о КО
     * @param sun_pos положение Солнца (необходимо для вычисления яркости)
     * @return массив структур с данными о КО
     */
    [[nodiscard]] inline virtual std::vector<SatState>
    check_sat_array(const ScopeState &scope_state,
                    std::vector<SatState> &sat_state, const Eigen::Vector3d &sun_pos) = 0;

    /**
     * находит массив звезд, входящих в конус обзора, параллельно вычисляя их положение в плоскости предметов и яркость
     * @param scope_dir направление оси телескопа
     * @return массив структур с данными о звездах
     */
    [[nodiscard]] inline virtual std::vector<Star>
    view_star_array(const ScopeState &scope_state) const = 0;

    /**
     * находит массив номеров зон (внутренняя сетка каталога), которые задеваются конусом обзора
     * @param scope_dir направление оси телескопа
     * @return массив номеров зон
     */
    [[nodiscard]] inline virtual std::vector<int>
    view_area_nums(const Eigen::Vector3d &scope_dir) const = 0;

    /**
     * геттер угла раствора конуса обзора
     * @return угол раствора конуса обзора
     */
    [[nodiscard]] inline double get_cone_angle() const {
        return cone_angle;
    }
}; // namespace Polygon
}

#endif //POLYGON_BASEVISUALFIELD_HPP
