//
// Created by leeiozh on 7/7/23.
//

#include <iostream>
#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(const std::shared_ptr<FileReader> &file_reader,
                         const std::shared_ptr<DispersionDirect> &area_search,
                         const std::shared_ptr<Mesh> &mesh,
                         const std::shared_ptr<DispersionCurve> &curve, bool change_std) :
        file_reader(file_reader), disp_direct(area_search), mesh(mesh), curve(curve),
        change_std(change_std) {
    mean_output.resize(MEAN);
    last_back.resize(NUM_STD);
    area_vec.resize(NUM_AREA);
    dir_vec.resize(CHANGE_DIR_NUM_SHOTS);
    hgd.resize(MEAN);
    speed.resize(MEAN);
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
        res[i] = get_mean_output();
    }
    return res;
}

OutputStructure MainProcess::run(const std::string &name) {

    index = 0;

    for (int i = 0; i < FOUR_NUM + 2 * MEAN; ++i) {
        InputStructure inp = file_reader->read_next_file(name, index);
        update(inp, name);
    }

    return get_mean_output();
}

void MainProcess::update(const InputStructure &input, const std::string &name) {

    speed[index % MEAN] = input.sog;
    hgd[index % MEAN] = input.giro;
    last_back[index % NUM_STD] = input.bcksctr;
    Eigen::VectorXi new_ang = disp_direct->calc_directions(last_back[index % NUM_STD], name, index);
    dir_vec[index % CHANGE_DIR_NUM_SHOTS] = new_ang[0];
    int med = 0;

    if (index % CHANGE_DIR_NUM_SHOTS == 0) {
        med = get_median_direction(dir_vec, true);
        auto inp_back = mesh->calc_back(area_vec[med], input.bcksctr);
        curve->update(index, inp_back, name);
    } else if (index >= MEAN) {
        med = get_median_direction(dir_vec, false);
        auto inp_back = mesh->calc_back(area_vec[med], input.bcksctr);
        curve->update(index, inp_back, name);
    }

    std::cout << " " << index << " med " << med << " new_angs " << new_ang.transpose() << " ";

    if (index >= FOUR_NUM + MEAN) std::cout << "next " << index << std::endl;

    index += 1;
    if (index >= 2 * FOUR_NUM) {
        index = FOUR_NUM; // avoid overflowing
    }
    mean_output[index % MEAN] = make_output();
    std::cout << " lens " << mean_output[index % MEAN].len.transpose() << " ";
}

OutputStructure MainProcess::make_output() {
    OutputStructure res;
    res.rose = disp_direct->get_rose();
    res.len = disp_direct->get_len();

    WaveStruct spec = curve->get_params();
    res.freq_spec = spec.freq_spec;

    for (int i = 0; i < NUM_SYSTEMS; i++) {

        if ((spec.m0[i] <= 1e-3) and (std::abs(spec.vcosalpha[i]) <= 1e-6)) { // / speed.mean()
            res.per[i] = 0.;
            res.swh[i] = 0.;
            res.m0[i] = 0.;
            res.dir[i] = 0.;
            res.vcos[i] = 0.;

        } else if ((spec.m0[i] <= 2e-3) and (std::abs(spec.vcosalpha[i]) > 2.)) {
            res.per[i] = 0.;
            res.swh[i] = 0.;
            res.m0[i] = 0.;
            res.dir[i] = 0.;
            res.vcos[i] = 0.;

        } else {
            res.per[i] = spec.peak_period[i];
            res.swh[i] = A_COEFF + B_COEFF * std::sqrt(spec.m0[i]);
            res.m0[i] = spec.m0[i];

            // correct on main wave has passing or opposite direction
            double dir = static_cast<double>(disp_direct->get_curr_std()[i]) / NUM_AREA * 360.;
            double phi = dir - hgd.mean();
            if (phi > 360.) phi -= 360.;
            else if (phi < 0.) phi += 360.;

            if ((spec.vcosalpha[i] < 0 and phi > 90. and phi < 270.) or
                (spec.vcosalpha[i] > 0 and (phi < 90. or phi > 270.))) {
                res.dir[i] = dir;
            } else {
                if (dir > 180.) res.dir[i] = dir - 180.;
                else res.dir[i] = dir + 180.;
            }
            res.vcos[i] = spec.vcosalpha[i];
        }
    }

    return res;
}

OutputStructure MainProcess::get_mean_output() {

    OutputStructure res;
    auto size = static_cast<double>(mean_output.size());

    for (auto &j: mean_output) {
        res.freq_spec += j.freq_spec / size;
        res.rose += j.rose / size;
        for (int i = 0; i < NUM_SYSTEMS; ++i) {
            res.swh[i] += j.swh[i] / size;
            res.per[i] += j.per[i] / size;
            res.dir[i] += j.dir[i] / size;
            res.len[i] += j.len[i] / size;
            res.m0[i] += j.m0[i] / size;
            res.vcos[i] += j.vcos[i] / size;
        }
    }

    return res;
}

int MainProcess::get_median_direction(const Eigen::VectorXi &dir_ind, bool change_mean) {

    if (change_mean) {
        auto copy_vec = Eigen::VectorXi(dir_ind);
        std::sort(copy_vec.begin(), copy_vec.end());
        return copy_vec[static_cast<int>(std::round(static_cast<double>(CHANGE_DIR_NUM_SHOTS) / 2))];
    } else {
        auto copy_vec = Eigen::VectorXi(dir_ind.segment(0, MEAN));
        std::sort(copy_vec.begin(), copy_vec.end());
        return copy_vec[static_cast<int>(std::round(static_cast<double>(MEAN) / 2))];
    }
}

} // namespace