//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_MAINPROCESS_HPP
#define SEAVISION_MAINPROCESS_HPP

#include <vector>
#include <memory>
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/Output/OutputStructure.hpp"

namespace SeaVision {

class MainProcess {

private:
    std::array<std::shared_ptr<DispersionCurve>, 3> curve_vec;

public:
    explicit MainProcess(std::array<std::shared_ptr<DispersionCurve>, 3>& curve_vec);

    OutputStructure get_out();

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
