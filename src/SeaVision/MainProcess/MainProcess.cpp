//
// Created by leeiozh on 7/7/23.
//

#include <iostream>
#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(const std::shared_ptr<FileReader> &file_reader, const std::shared_ptr<AreaSearch> &area_search,
                         const std::shared_ptr<Mesh> &mesh,
                         const std::vector<std::shared_ptr<DispersionCurve>> &curve_vec, bool change_std) :
                         file_reader(file_reader), area_search(area_search), mesh(mesh), curve_vec(curve_vec),
                         change_std(change_std) {
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

OutputStructure MainProcess::run(const std::string &name) {

    index = 0;

    for (int i = 0; i < FOUR_NUM + 5; ++i) {
        InputStructure inp = file_reader->read_next_file(name, index);
        update(inp);
    }

    return get_out();
}

void MainProcess::update(const InputStructure &input) {

    last_back[index % NUM_STD] = input.bcksctr;
    int std_ang = 0;

    if (index >= NUM_STD) {
        if (change_std) {
            std_ang = area_search->search_area(last_back);
        } else {
            if (index == NUM_STD) {
                std_ang = area_search->search_area(last_back);
            } else {
                std_ang = area_search->get_curr_az_ind();
            }
        }
        auto inp_back = mesh->calc_back(area_vec[std_ang], input.bcksctr);
        curve_vec[0]->update(index, inp_back);

    } else {
        auto inp_back = mesh->calc_back(area_vec[0], input.bcksctr);
        curve_vec[0]->update(index, inp_back);
    }

    std::cout << " " << index << " " << std_ang << " ";
    //if (index % 32 == 0) std::cout << "% 32 " << static_cast<double>(index) / static_cast<double>(FOUR_NUM) * 100 << std::endl;
    if (index >= FOUR_NUM + NUM_STD) std::cout << "next " << index << std::endl;

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
        res.m0 = spec.m0;
    }
    return res;
}

} // namespace