//
// Created by leeiozh on 7/7/23.
//

#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(std::array<std::shared_ptr<DispersionCurve>, 3> &curve_vec) : curve_vec(curve_vec) {}

OutputStructure MainProcess::get_out() {
    for (int i = 0; i < 3; ++i){
        curve_vec[i].get();
    }

}

} // namespace