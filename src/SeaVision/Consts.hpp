//
// Created by leeiozh on 7/6/23.
//

#ifndef SEAVISION_CONSTS_HPP
#define SEAVISION_CONSTS_HPP

#include <cmath>

namespace SeaVision {

const int NUM_SYSTEMS = 3;

const double TURN_PERIOD_DEF = 2.5; // period of radars turn in seconds
extern double TURN_PERIOD;

const double MAX_WAIT_SHOT_TIME_DEF = 2 * TURN_PERIOD_DEF; // period of waiting one radars turn in seconds
extern double MAX_WAIT_SHOT_TIME;

const int CUT_NUM = 32; // number of lines taken into account after 2d fourier transform

const int AREA_SIZE = 720; // width and height of area square in meters

const int AREA_DISTANCE_METERS = 840; //

const int AREA_DISTANCE_PX = 448;

const int AREA_AZIMUTH_PX = 4096;

const double K_MAX = 2. * M_PI / static_cast<double>(AREA_SIZE) * static_cast<double>(CUT_NUM); // max wave number

const double G_COEFF = 9.81; // gravity acceleration

const int DELTA_FREQ = 13; // width of window for trimming a dispersion curve

const int FOUR_NUM = 256; // number of shots for Welch transform (must be a poser of two)

const double BETA = -1.2; // magic power of wave number (see Borge, 2008)

const double A_COEFF = 0.42975475; // coefficient in SWH = A + B \sqrt(SNR)

const double B_COEFF = 4.47947967; // coefficient in SWH = A + B \sqrt(SNR)

const double C_COEFF = 0.42975475; // coefficient in SWH = C + D \sqrt(MAX_DISP)

const double D_COEFF = 4.47947967; // coefficient in SWH = C + D \sqrt(MAX_DISP)

const double STEP = 1.875; // resolution of image (distance in meters between neighbors cells)

const int CHANGE_DIR_NUM_SHOTS = 128;

const int NUM_AREA_DEF = 32; // number of sectors by azimuth
extern int NUM_AREA;

const int MEAN_DEF = 16; // number of results for averaging (must be a power of two!)
extern int MEAN;

}

#endif //SEAVISION_CONSTS_HPP
