//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_FUNCTIONS_HPP
#define POLYGON_FUNCTIONS_HPP

#include "Structs.hpp"
#include <random>

namespace Polygon {

double calc_jp(const double &angle) {
    return std::cos(angle);
}

double normal_random(const NoiseParams &noiseParams) {
    int sum_noise = noiseParams.count + noiseParams.warm;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> dist(0, sum_noise);
    return dist(gen);
}

}

#endif //POLYGON_FUNCTIONS_HPP
