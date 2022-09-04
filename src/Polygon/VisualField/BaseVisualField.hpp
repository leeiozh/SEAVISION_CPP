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
protected:
    double cone_angle;
    Eigen::Vector2d view_sizes;
    std::string path_to_res = "resources";

public:

    inline explicit BaseVisualField(const double &cone_angle, Eigen::Vector2d view_sizes) : cone_angle(
            cone_angle), view_sizes(std::move(view_sizes)) {
        if (cone_angle > M_PI_2 || cone_angle < 0) {
            std::stringstream buff;
            buff << "Угол раствора конуса обзора может принимать значения из [0, pi/2]! Текущий угол: "
                 << cone_angle << " радиан!" << std::endl;
            throw PolygonBaseException(buff.str().c_str());
        }
    };

    inline BaseVisualField(const double &cone_angle, Eigen::Vector2d view_sizes, std::string path) :
            path_to_res(std::move(path)), cone_angle(cone_angle), view_sizes(std::move(view_sizes)) {};

    [[nodiscard]] inline virtual bool
    check_sat(const ScopeState &scope_state,
              const Eigen::Vector3d &sat_pos) const = 0;

    [[nodiscard]] inline virtual std::vector<SatState>
    check_sat_array(const ScopeState &scope_state,
                    std::vector<SatState> &sat_state, const Eigen::Vector3d &sun_pos) = 0;

    [[nodiscard]] inline virtual std::vector<Star>
    view_star_array(const Eigen::Vector3d &scope_dir) const = 0;

    [[nodiscard]] inline virtual std::vector<int>
    view_area_nums(const Eigen::Vector3d &scope_dir) const = 0;

    [[nodiscard]] inline double get_cone_angle() const {
        return cone_angle;
    }
}; // namespace Polygon
}

#endif //POLYGON_BASEVISUALFIELD_HPP
