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
    int index = 0;
    std::shared_ptr<FileReader> file_reader;
    std::shared_ptr<AreaSearch> area_search;
    std::shared_ptr<Mesh> mesh;
    std::vector<Eigen::MatrixXi> last_back;
    std::vector<Area> area_vec;
    std::vector<std::shared_ptr<DispersionCurve>> curve_vec;

public:
    MainProcess(const std::shared_ptr<FileReader>& file_reader, const std::shared_ptr<AreaSearch> &area_search,
                const std::shared_ptr<Mesh> &mesh, const std::vector<std::shared_ptr<DispersionCurve>> &curve_vec);

    void update(const InputStructure &input);

    std::vector<OutputStructure> run(int num);

    OutputStructure get_out();

};

} // namespace

#endif //SEAVISION_MAINPROCESS_HPP
