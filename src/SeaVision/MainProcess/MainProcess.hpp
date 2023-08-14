//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_MAINPROCESS_HPP
#define SEAVISION_MAINPROCESS_HPP

#include <vector>
#include <memory>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/DispersionStage/DispersionDirect.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/Structures.hpp"

namespace SeaVision {

class MainProcess {

private:
    int index = 0; // index of current shot
    Eigen::VectorXd hgd; // current course from vessel gyroscope
    Eigen::VectorXd speed; // current speed from vessel gyroscope
    std::shared_ptr<FileReader> file_reader; // pointer on file reader
    std::shared_ptr<DispersionDirect> disp_direct; // pointer on area searcher
    std::shared_ptr<Mesh> mesh; // pointer on mesh
    std::vector<Eigen::MatrixXi> last_back; // saved last STD_NUM backscatters
    std::vector<Area> area_vec; // vector of areas for speedy computations
    std::shared_ptr<DispersionCurve> curve; // pointer on fourier stuff
    bool change_std = true; // if false then we do not change place of clearly data (using for comparing with other short measurements)

    Eigen::VectorXi dir_vec;
    std::vector<OutputStructure> mean_output;

public:
    /**
     * constructor
     * @param file_reader pointer on file reader
     * @param area_search  pointer on area searcher
     * @param mesh pointer on mesh
     * @param curve_vec pointer on fourier stuff
     * @param change_std if false then we do not change place of clearly data (using for comparing with other short measurements)
     */
    MainProcess(const std::shared_ptr<FileReader> &file_reader, const std::shared_ptr<DispersionDirect> &area_search,
                const std::shared_ptr<Mesh> &mesh, const std::shared_ptr<DispersionCurve> &curve,
                bool change_std);

    /**
     * update and process next shot
     * @param input InputStructure of current shot
     * @param name TODO delete
     */
    void update(const InputStructure &input, const std::string &name);

    /**
     * running processing queue of shots
     * @param num number of processing shots
     * @param name TODO delete
     * @return vector of OutputStructures with results of all processing shots
     */
    std::vector<OutputStructure> run(int num, const std::string &name);

    /**
     * running processing next shot
     * @param name TODO delete
     * @return OutputStructures with results
     */
    OutputStructure run(const std::string &name);

    /**
     * getter of last results
     * @return OutputStructure with results
     */
    OutputStructure make_output();

    OutputStructure get_mean_output();

    static int get_median_direction(const Eigen::VectorXi &dir_ind, bool change_mean);

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
