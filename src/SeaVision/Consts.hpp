//
// Created by leeiozh on 7/6/23.
//

#ifndef SEAVISION_CONSTS_HPP
#define SEAVISION_CONSTS_HPP

#include <cmath>

namespace SeaVision {

constexpr int NUM_SYSTEMS = 3; // number of wave systems taken into account

constexpr double TURN_PERIOD = 2.5; // period of radars turn in seconds

constexpr int CUT_NUM = 32; // number of lines taken into account after 2d fourier transform

constexpr int AREA_SIZE = 720; // width and height of area square in meters

constexpr double K_MAX = 2. * M_PI / static_cast<double>(AREA_SIZE) * static_cast<double>(CUT_NUM); // max wave number

constexpr double G_COEFF = 9.81; // gravity acceleration

constexpr double DELTA_FREQ = 13; // width of window for trimming a dispersion curve

constexpr int FOUR_NUM = 256; // number of shots for Welch transform (must be a poser of two)

constexpr double BETA = -1.2; // magic power of wave number (see Borge, 2008)

constexpr double A_COEFF = 0.42975475; // coefficient in SWH = A + B \sqrt(SNR)

constexpr double B_COEFF = 4.47947967; // coefficient in SWH = A + B \sqrt(SNR)

constexpr double C_COEFF = 0.42975475; // coefficient in SWH = C + D \sqrt(MAX_DISP)

constexpr double D_COEFF = 4.47947967; // coefficient in SWH = C + D \sqrt(MAX_DISP)

constexpr int NUM_AREA = 32; // number of sectors by azimuth

constexpr int NUM_STD = 4; // number of shots for calculating a zone with the most contrast signal

constexpr double STEP = 1.875; // resolution of image (distance in meters between neighbors cells)

constexpr int CHANGE_DIR_NUM_SHOTS = 256;

constexpr int MEAN = 16; // number of results for averaging (must be a power of two!)

}

#endif //SEAVISION_CONSTS_HPP
