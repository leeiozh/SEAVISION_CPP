////
//// Created by leeiozh on 01.09.22.
////
//
//#ifndef POLYGON_CONVERTERS_HPP
//#define POLYGON_CONVERTERS_HPP
//
//#include "../Utility/Consts.hpp"
//#include "../Utility/Structs.hpp"
//#include "../Utility/Functions.hpp"
//
//namespace Polygon {
//
//double intens_to_electrons(const double &intens, const double &time_expose, const double &light_length,
//                           const double &quant_eff,
//                           const MatrixParams &matrixParams) {
//    double res = intens * time_expose * matrixParams.pix_x * matrixParams.pix_y * light_length / PLUNK_CONSTANT_34 /
//                 LIGHT_SPEED * 1e25;
//    res *= quant_eff;
//    if (res > matrixParams.pix_capacity) {
//        return matrixParams.pix_capacity;
//    }
//    return res;
//}
//
//double intens_to_electrons_with_noise(const double &intens, const double &time_expose, const double &light_length,
//                                      const double &quant_eff,
//                                      const MatrixParams &matrixParams) {
//    double res = intens * time_expose * matrixParams.pix_x * matrixParams.pix_y * light_length / PLUNK_CONSTANT_34 /
//                 LIGHT_SPEED * 1e25;
//    res *= quant_eff;
//
//    res += normal_random(matrixParams.noise_params);
//
//    if (res > matrixParams.pix_capacity) {
//        return matrixParams.pix_capacity;
//    } else if (res < 0) {
//        return 0;
//    }
//    return res;
//}
//
//}
//#endif //POLYGON_CONVERTERS_HPP
