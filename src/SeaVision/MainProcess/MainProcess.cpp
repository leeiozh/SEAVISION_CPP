//
// Created by leeiozh on 7/7/23.
//

#include <iostream>
#include <chrono>
#include "MainProcess.hpp"

namespace SeaVision {

MainProcess::MainProcess(const std::shared_ptr<InputProcessor> &inp_proc,
                         const std::shared_ptr<OutputProcessor> &output_proc,
                         const std::shared_ptr<DispersionDirect> &disp_direct,
                         const std::shared_ptr<Mesh> &mesh,
                         const std::shared_ptr<DispersionCurve> &curve) :
        inp_proc(inp_proc), output_proc(output_proc), disp_direct(disp_direct), mesh(mesh), curve(curve) {

    mean_output.resize(MEAN);
    area_vec.resize(NUM_AREA);
    dir_vec.resize(CHANGE_DIR_NUM_SHOTS);
    hgd.resize(MEAN);
    speed.resize(MEAN);
    curr_dir = 0;

    for (int i = 0; i < NUM_AREA; ++i) { // precompile matrix for convert from polar to cartesian
        double angle = -static_cast<double >(i) / static_cast<double>(NUM_AREA) * 360.;
        area_vec[i] = Area(AREA_SIZE, AREA_SIZE, -angle, angle, AREA_DISTANCE_METERS);
    }
}

/*std::vector<OutputStructure> MainProcess::run_debug(int num) {

    index = 0;
    for (int i = 0; i < FOUR_NUM; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp);
    }

    std::vector<OutputStructure> res(num);
    for (int i = 0; i < num; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp);
        res[i] = get_mean_output();
    }
    return res;
}

OutputStructure MainProcess::run_debug() {

    index = 0;

    for (int i = 0; i < FOUR_NUM + 2 * MEAN; ++i) {
        InputStructure inp = file_reader->read_next_file(index);
        update(inp);
    }

    return get_mean_output();
}*/

void MainProcess::run_realtime() {

    index = 0;
    std::queue<InputStructure> data_queue; // queue of last not more MEAN results
    std::mutex mtx; // mutex for changing queue when input proccesor want to change it

    std::cout << "start" << std::endl;

    std::thread reader_thread([&]() { // thread for reading data and convert it to InputStructure
        while (true) { // infinity loop
            try {
                const auto start = std::chrono::steady_clock::now();

                InputStructure inp = inp_proc->listen_message();

                const auto end = std::chrono::steady_clock::now();
                const std::chrono::duration<double> elapsed_seconds = end - start;

                std::cout << index << " " << data_queue.size() << " read " << elapsed_seconds.count() << std::endl;
                {
                    std::lock_guard<std::mutex> lock(mtx);

                    // if queue overflowed delete the oldest element
                    if (data_queue.size() >= MEAN) data_queue.pop();
                    data_queue.push(inp); // add new element
                }
                // std::this_thread::sleep_for(std::chrono::seconds(1));
            } catch (const SeaVisionException &exception) {
                std::cerr << "Read error: " << exception.what() << std::endl;
                continue;
            }
        }
    });

    std::thread processor_thread([&]() {
        while (true) {
            try {
                InputStructure local_copy; // copy of the oldest element in queue
                bool change = false; // flag if proccesor works speedier than sender, queue must be not empty for next processing

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (!data_queue.empty()) {
                        local_copy = data_queue.front(); // copy the oldest element
                        data_queue.pop(); // remove it from queue
                        change = true; // we detected changes, we can process
                    }
                }

                if (change) {

                    const auto start = std::chrono::steady_clock::now();
                    update(local_copy); // process data
                    const auto end = std::chrono::steady_clock::now();
                    const std::chrono::duration<double> elapsed_seconds = end - start;

                    std::cout << index << " " << data_queue.size() << " update " << elapsed_seconds.count()
                              << std::endl;

                    if (index > FOUR_NUM + 2 * MEAN) { // TODO check it
                        OutputStructure res = get_mean_output(); // prepare results for sending
                        output_proc->pass_one_message(res); // sending
                    }
                }

            } catch (const SeaVisionException &exception) {
                std::cerr << "Proccess error: " << exception.what() << std::endl;
                continue;
            }
        }
    });

    reader_thread.join();
    processor_thread.join();
}

void MainProcess::update(const InputStructure &input) {

    speed[index % MEAN] = input.cond.sog; // update sog
    hgd[index % MEAN] = input.cond.cog; // update cog

    Eigen::VectorXi new_ang = disp_direct->calc_directions(input.prli.bcksctr, index); // find index of zone with the most contrast signal
    dir_vec[index % CHANGE_DIR_NUM_SHOTS] = new_ang[0]; // update main direction where after we do Fourier and Welch transform

    if (index == MEAN) curr_dir = get_median_direction(dir_vec, false);
    else if (index % CHANGE_DIR_NUM_SHOTS == 0) curr_dir = get_median_direction(dir_vec, true);

    auto inp_back = mesh->calc_back(area_vec[curr_dir], input.prli.bcksctr);
    curve->update(index, inp_back);

    index += 1;
    if (index >= 2 * FOUR_NUM) {
        index = FOUR_NUM; // avoid overflowing
    }

    mean_output[index % MEAN] = make_output();
}

OutputStructure MainProcess::make_output() {
    OutputStructure res;
    res.rose = disp_direct->get_rose();
    res.len = disp_direct->get_len();

    WaveStruct spec = curve->get_params();
    res.freq_spec = spec.freq_spec;

    for (int i = 0; i < NUM_SYSTEMS; i++) {

        bool cond = ((spec.m0[i] <= 1e-3) and (std::abs(spec.vcosalpha[i]) <= 1e-6))
                    or ((spec.m0[i] <= 2e-3) and (std::abs(spec.vcosalpha[i]) > 2.)); // / speed.mean()

        if (cond) {
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

} // namespace