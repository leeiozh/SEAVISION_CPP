//
// Created by leeiozh on 7/7/23.
//

#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(std::array<std::shared_ptr<DispersionCurve>, 3> &curve_vec) : curve_vec(curve_vec) {}

OutputStructure MainProcess::get_out() {
    OutputStructure res;
    for (int i = 0; i < 3; ++i){
        SpectrumStruct spec = curve_vec[i]->get_params();
        res.per[i] = spec.peak_period;
        res.swh[i] = A_COEFF + B_COEFF * std::sqrt(spec.m0);
    }

}

} // namespace