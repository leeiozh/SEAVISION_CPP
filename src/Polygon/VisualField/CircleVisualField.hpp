//
// Created by leeiozh on 05.08.22.
//

#ifndef POLYGON_CIRCLEVISUALFIELD_HPP
#define POLYGON_CIRCLEVISUALFIELD_HPP

#include <utility>
#include "BaseVisualField.hpp"

namespace Polygon {

class CircleVisualField : public BaseVisualField {

public:

    explicit CircleVisualField(double cone_angle) : BaseVisualField(cone_angle) {};

    CircleVisualField(double cone_angle, std::string path) : BaseVisualField(std::move(path), cone_angle) {};

    [[nodiscard]] bool check_sat(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                   const Eigen::Vector3d &sat_pos) const override;

    [[nodiscard]] std::vector<SatState> check_sat_array(const Eigen::Vector3d &scope_dir, const Eigen::Vector3d &scope_pos,
                                                         const std::vector<SatState> &sat_state) const override;

    [[nodiscard]] std::vector<int> view_area_nums(const Eigen::Vector3d &scope_dir) const override;

    [[nodiscard]] std::vector<Star>
    view_star_array(const Eigen::Vector3d &scope_dir) const override;
};

} // namespace Polygon


#endif //POLYGON_CIRCLEVISUALFIELD_HPP
