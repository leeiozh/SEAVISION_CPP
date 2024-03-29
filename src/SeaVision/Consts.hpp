//
// Created by leeiozh on 7/6/23.
//

#ifndef SEAVISION_CONSTS_HPP
#define SEAVISION_CONSTS_HPP

#include <cmath>

namespace SeaVision {

const int NUM_SYSTEMS = 3;

extern bool FOUR_OR_DISP;

const double TURN_PERIOD_DEF = 2.5; // period of radars turn in seconds
extern double TURN_PERIOD;

extern double MAX_WAIT_SHOT_TIME; // period of waiting one radars turn in seconds

const int CUT_NUM = 32; // number of lines taken into account after 2d fourier transform

const double AREA_SIZE_DEF = 720;
extern double AREA_SIZE; // width and height of area square in meters

const double AREA_DISTANCE_METERS_DEF = 840;
extern double AREA_DISTANCE_METERS;

const int AREA_DISTANCE_PX = 448;

const int AREA_AZIMUTH_PX = 4096;

const int AREA_READ_DIST_PX = 1024;

const double K_MAX_DEF = 2. * M_PI / static_cast<double>(AREA_SIZE_DEF) * static_cast<double>(CUT_NUM);
extern double K_MAX; // max wave number

const double G_COEFF = 9.81; // gravity acceleration

const int DELTA_FREQ = 13; // width of window for trimming a dispersion curve

const int FOUR_NUM = 256; // number of shots for Welch transform (must be a power of two)

const double BETA = -1.2; // magic power of wave number (see Borge, 2008)

const double A_COEFF = 0.95655; // coefficient in SWH = A + B \sqrt(SNR)

const double B_COEFF = 4.62937; // coefficient in SWH = A + B \sqrt(SNR)

const double C_COEFF = 0.2574829; // coefficient in SWH = C + D \sqrt(MAX_DISP)

const double D_COEFF = 2.4171893; // coefficient in SWH = C + D \sqrt(MAX_DISP)

const double STEP_DEF = 1.875; // resolution of image (distance in meters between neighbors cells)

const int CHANGE_DIR_NUM_SHOTS = 64;

const int NUM_AREA_DEF = 32; // number of sectors by azimuth
extern int NUM_AREA;

const int MEAN_DEF = 16; // number of results for averaging (must be a power of two!)
extern int MEAN;

}

#endif //SEAVISION_CONSTS_HPP
