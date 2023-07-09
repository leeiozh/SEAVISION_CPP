//
// Created by leeiozh on 7/7/23.
//

#include <iostream>
#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(const std::shared_ptr<FileReader> &file_reader, const std::shared_ptr<AreaSearch> &area_search,
                         const std::shared_ptr<Mesh> &mesh,
                         const std::vector<std::shared_ptr<DispersionCurve>> &curve_vec) : file_reader(file_reader),
                                                                                           area_search(area_search),
                                                                                           mesh(mesh),
                                                                                           curve_vec(curve_vec) {
    last_back.resize(NUM_STD);
    area_vec.resize(NUM_AREA);
    for (int i = 0; i < NUM_AREA; ++i) {
        double angle = -static_cast<double >(i) / static_cast<double>(NUM_AREA) * 360.;
        area_vec[i] = Area(720, 720, -angle, angle, 840);
    }
}

std::vector<OutputStructure> MainProcess::run(int num) {

    index = 0;
    for (int i = 0; i < FOUR_NUM; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp);
    }

    std::vector<OutputStructure> res(num);
    for (int i = 0; i < num; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp);
        res[i] = get_out();
    }
    return res;
}

void MainProcess::update(const InputStructure &input) {

    last_back[index % NUM_STD] = input.bcksctr;

    if (index >= NUM_STD) {
        int std_ang = area_search->search_area(last_back);
        auto inp_back = mesh->calc_back(area_vec[std_ang], input.bcksctr);
        curve_vec[0]->update(index, inp_back);
    } else {
        auto inp_back = mesh->calc_back(area_vec[0], input.bcksctr);
        curve_vec[0]->update(index, inp_back);
    }

    if (index % 64 == 0) std::cout << "% 64 " << index / FOUR_NUM * 100 << std::endl;
    if (index >= FOUR_NUM) std::cout << "next " << index << std::endl;

    index += 1;
    if (index >= 512) {
        index = 256;
    }
}

OutputStructure MainProcess::get_out() {
    OutputStructure res;
    for (int i = 0; i < curve_vec.size(); ++i) {
        SpectrumStruct spec = curve_vec[i]->get_params();
        res.per[i] = spec.peak_period;
        res.swh[i] = A_COEFF + B_COEFF * std::sqrt(spec.m0);
        res.freq_spec = spec.freq_spec;
    }
    return res;
}

} // namespace