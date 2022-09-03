//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_BASEVISUALFIELD_HPP
#define POLYGON_BASEVISUALFIELD_HPP

#include "../Utility/Structs.hpp"
#include "../PoligonBaseException.hpp"
#include <utility>
#include <vector>
#include <eigen3/Eigen/Core>

namespace Polygon {

class BaseVisualField {
protected:
    double cone_angle;
    std::string path_to_res = "resources";

public:

    explicit BaseVisualField(double cone_angle) : cone_angle(cone_angle) {
        if (cone_angle > M_PI_2 || cone_angle < 0) {
            std::stringstream buff;
            buff << "Угол раствора конуса обзора может принимать значения из [0, pi/2]! Текущий угол: "
                 << cone_angle << " радиан!" << std::endl;
            throw PolygonBaseException(buff.str().c_str());
        }
    };

    BaseVisualField(std::string path, double cone_angle) : path_to_res(std::move(path)), cone_angle(cone_angle) {};

    [[nodiscard]] virtual bool
    check_sat(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
              const Eigen::Vector3d &sat_pos) const = 0;

    [[nodiscard]] virtual std::vector<SatState>
    check_sat_array(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                    std::vector<SatState> &sat_state, const Eigen::Vector3d &sun_pos) = 0;

    [[nodiscard]] virtual std::vector<Star>
    view_star_array(const Eigen::Vector3d &scope_dir) const = 0;

    [[nodiscard]] virtual std::vector<int>
    view_area_nums(const Eigen::Vector3d &scope_dir) const = 0;

    [[nodiscard]] double get_cone_angle() const {
        return cone_angle;
    }
}; // namespace Polygon
}

#endif //POLYGON_BASEVISUALFIELD_HPP
