//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_MAINPROCESS_HPP
#define SEAVISION_MAINPROCESS_HPP

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Input/InputProcessor.hpp"
#include "SeaVision/Output/OutputProcessor.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/DispersionStage/DispersionDirect.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/Structures.hpp"

namespace SeaVision {

class MainProcess {

private:
    int index = 0;                                      // index of current shot
    std::vector<OutputStructure> mean_output;           // vector of MEAN last results

    Eigen::VectorXd hgd = Eigen::VectorXd::Zero(MEAN);        // last MEAN values of course from vessel gyroscope
    Eigen::VectorXd speed = Eigen::VectorXd::Zero(MEAN);      // last MEAN values of speed from vessel navigation
    Eigen::VectorXi dir_vec = Eigen::VectorXi::Zero(CHANGE_DIR_NUM_SHOTS); // vector of CDNS last main directions
    int curr_dir = 0;                                   // index of current zone with the most contrast signal
    double curr_step = STEP_DEF;

    std::unique_ptr<InputProcessor> inp_proc;           // pointer on input processor
    std::unique_ptr<OutputProcessor> output_proc;       // pointer on output processor

    std::unique_ptr<DispersionDirect> disp_direct;      // pointer on dispersion calculator
    std::unique_ptr<DispersionCurve> curve;             // pointer on fourier calculator

    std::unique_ptr<Mesh> mesh;                         // pointer on mesh
    std::vector<Area> area_vec;                         // vector of areas for speedy computations
    bool four_or_disp = true;

public:
    /**
     * constructor
     * @param inp_proc      pointer on input processor
     * @param output_proc   pointer of output processor
     * @param disp_direct   pointer on area searcher
     * @param mesh          pointer on mesh
     * @param curve_vec     pointer on fourier stuff
     */
    MainProcess(std::unique_ptr<InputProcessor> inp_proc, std::unique_ptr<OutputProcessor> output_proc,
                std::unique_ptr<DispersionDirect> disp_direct, std::unique_ptr<Mesh> mesh,
                std::unique_ptr<DispersionCurve> curve, bool four_or_disp);

    /**
     * update and process next shot
     * @param input InputStructure of current shot
     */
    void update(const InputStructure &input);

    /**
     * main function to run a program
     */
    void run_realtime();

    /**
     * remeshing for current resolution
     * @param step input step of backscatter
     */
    void remesh(double step);

    /**
     * getter of last results
     * @return OutputStructure with results
     */
    OutputStructure make_output();

    /**
     * getter of average MEAN result
     * @return OutputStructure with average results
     */
    OutputStructure get_mean_output();

    /**
     * running processing queue of shots
     * @param num number of processing shots
     * @return vector of OutputStructures with results of all processing shots
     */
    //std::vector<OutputStructure> run_debug(int num);

    /**
     * running processing next shot
     * @return OutputStructures with results
     */
    //OutputStructure run_debug();

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
