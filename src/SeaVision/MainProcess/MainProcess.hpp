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
    int index = 0; // index of current shot

    Eigen::VectorXd hgd; // last MEAN course from vessel gyroscope
    Eigen::VectorXd speed; // last MEAN speed from vessel navigation

    std::shared_ptr<InputProcessor> inp_proc; // pointer on input proccesor
    std::shared_ptr<OutputProcessor> output_proc; // pointer on output proccesor

    std::shared_ptr<DispersionDirect> disp_direct; // pointer on area searcher
    std::shared_ptr<Mesh> mesh; // pointer on mesh
    std::vector<Area> area_vec; // vector of areas for speedy computations
    std::shared_ptr<DispersionCurve> curve; // pointer on fourier stuff

    Eigen::VectorXi dir_vec; // vector of CHANGE_DIR_NUM_SHOTS last main directions
    int curr_dir; // index of current zone with the most contrast signal
    std::vector<OutputStructure> mean_output; // vector of MEAN last results

public:
    /**
     * constructor
     * @param inp_proc pointer on input processor
     * @param output_proc pointer of output processor
     * @param disp_direct  pointer on area searcher
     * @param mesh pointer on mesh
     * @param curve_vec pointer on fourier stuff
     */
    MainProcess(const std::shared_ptr<InputProcessor> &inp_proc, const std::shared_ptr<OutputProcessor> &output_proc,
                const std::shared_ptr<DispersionDirect> &disp_direct, const std::shared_ptr<Mesh> &mesh,
                const std::shared_ptr<DispersionCurve> &curve);

    /**
     * update and process next shot
     * @param input InputStructure of current shot
     */
    void update(const InputStructure &input);

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

    void run_realtime();

    /**
     * getter of last results
     * @return OutputStructure with results
     */
    OutputStructure make_output();

    OutputStructure get_mean_output();

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
