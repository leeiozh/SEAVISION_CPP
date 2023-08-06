//
// Created by leeiozh on 7/7/23.
//

#ifndef SEAVISION_MAINPROCESS_HPP
#define SEAVISION_MAINPROCESS_HPP

#include <vector>
#include <memory>
#include "SeaVision/Input/FileReader.hpp"
#include "SeaVision/Interpolant/Mesh.hpp"
#include "SeaVision/Interpolant/AreaSearch.hpp"
#include "SeaVision/Input/InputStructure.hpp"
#include "SeaVision/FourierStage/DispersionCurve.hpp"
#include "SeaVision/Output/OutputStructure.hpp"

namespace SeaVision {

class MainProcess {

private:
    int index = 0; // index of current shot
    double hgd = 0.; // current course from vessel gyroscope
    std::shared_ptr<FileReader> file_reader; // pointer on file reader
    std::shared_ptr<AreaSearch> area_search; // pointer on area searcher
    std::shared_ptr<Mesh> mesh; // pointer on mesh
    std::vector<Eigen::MatrixXi> last_back; // saved last STD_NUM backscatters
    std::vector<Area> area_vec; // vector of areas for speedy computations
    std::vector<std::shared_ptr<DispersionCurve>> curve_vec; // vector of pointers on fourier stuff (3)
    bool change_std = true; // if false then we do not change place of clearly data (using for comparing with other short measurements)

public:
    /**
     * constructor
     * @param file_reader pointer on file reader
     * @param area_search  pointer on area searcher
     * @param mesh pointer on mesh
     * @param curve_vec vector of pointers on fourier stuff (3)
     * @param change_std if false then we do not change place of clearly data (using for comparing with other short measurements)
     */
    MainProcess(const std::shared_ptr<FileReader> &file_reader, const std::shared_ptr<AreaSearch> &area_search,
                const std::shared_ptr<Mesh> &mesh, const std::vector<std::shared_ptr<DispersionCurve>> &curve_vec,
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
    OutputStructure get_out();

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
