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

std::vector<OutputStructure> MainProcess::run(int num, const std::string &name) {

    index = 0;
    for (int i = 0; i < FOUR_NUM; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp, name);
    }

    std::vector<OutputStructure> res(num);
    for (int i = 0; i < num; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp, name);
        res[i] = get_out();
    }
    return res;
}

OutputStructure MainProcess::run(const std::string &name) {

    index = 0;

    for (int i = 0; i < FOUR_NUM + NUM_STD; ++i) {
        InputStructure inp = file_reader->read_next_file(name, index);
        update(inp, name);
    }

    return get_out();
}

void MainProcess::update(const InputStructure &input, const std::string &name) {

    hgd = input.giro;
    last_back[index % NUM_STD] = input.bcksctr;
    int std_ang = 0;
    int new_ang = 0;

    if (index >= NUM_STD) {
        if (change_std) {
            std_ang = area_search->search_area(last_back);
            new_ang = area_search->search_dir(last_back[NUM_STD - 1]);
        } else {
            if (index == NUM_STD) {
                std_ang = area_search->search_area(last_back);
                new_ang = area_search->search_dir(last_back[NUM_STD - 1]);
            } else {
                std_ang = area_search->get_curr_az_ind();
                new_ang = area_search->get_curr_std();
            }
        }
        auto inp_back = mesh->calc_back(area_vec[std_ang], input.bcksctr);
        curve_vec[0]->update(index, inp_back, name);

    } else {
        auto inp_back = mesh->calc_back(area_vec[0], input.bcksctr);
        curve_vec[0]->update(index, inp_back, name);
    }

    std::cout << " " << index << " " << std_ang << " " << new_ang << " ";
    //if (index % 32 == 0) std::cout << "% 32 " << static_cast<double>(index) / static_cast<double>(FOUR_NUM) * 100 << std::endl;
    if (index >= FOUR_NUM + NUM_STD) std::cout << "next " << index << std::endl;

    index += 1;
    if (index >= 512) {
        index = 256; // avoid overflowing
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
        res.rose = area_search->get_rose();

        // correct on wave has passing or opposite direction
        double dir = static_cast<double>(area_search->get_curr_std()) / NUM_AREA * 360.;
        double phi = dir - hgd;
        if (phi > 360.) phi -= 360.;
        else if (phi < 0.) phi += 360.;

        if ((spec.vcosalpha < 0 and phi > 90. and phi < 270.) or (spec.vcosalpha > 0 and (phi < 90. or phi > 270.))) {
            res.dir[i] = dir;
        } else {
            if (dir > 180.) res.dir[i] = dir - 180.;
            else res.dir[i] = dir + 180.;
        }

        res.vcos = spec.vcosalpha;
    }
    return res;
}

} // namespace