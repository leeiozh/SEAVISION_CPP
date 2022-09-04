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

public:

    inline explicit CircleVisualField(const double &cone_angle, Eigen::Vector2d view_sizes) : BaseVisualField(
            cone_angle, std::move(view_sizes)) {};

    inline CircleVisualField(const double &cone_angle, Eigen::Vector2d view_sizes, const std::string &path)
            : BaseVisualField(cone_angle, std::move(view_sizes), path) {};

    [[nodiscard]] bool check_sat(const ScopeState &scope_state,
                                 const Eigen::Vector3d &sat_pos) const override;

    [[nodiscard]] std::vector<SatState> check_sat_array(const ScopeState &scope_state,
                                                        std::vector<SatState> &sat_state,
                                                        const Eigen::Vector3d &sun_pos) override;

    [[nodiscard]] std::vector<int> view_area_nums(const Eigen::Vector3d &scope_dir) const override;

    [[nodiscard]] std::vector<Star>
    view_star_array(const Eigen::Vector3d &scope_dir) const override;
};

} // namespace Polygon


#endif //POLYGON_CIRCLEVISUALFIELD_HPP
