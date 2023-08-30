//
// Created by leeiozh on 7/7/23.
//

#include "MainProcess.hpp"

namespace SeaVision {

double AREA_SIZE, AREA_DISTANCE_METERS, K_MAX;

MainProcess::MainProcess(std::unique_ptr<InputProcessor> inp_proc, std::unique_ptr<OutputProcessor> output_proc,
                         std::unique_ptr<DispersionDirect> disp_direct, std::unique_ptr<Mesh> mesh,
                         std::unique_ptr<DispersionCurve> curve, bool four_or_disp) :
        inp_proc(std::move(inp_proc)), output_proc(std::move(output_proc)), disp_direct(std::move(disp_direct)),
        mesh(std::move(mesh)), curve(std::move(curve)), four_or_disp(four_or_disp) {

    mean_output.resize(MEAN);                  // default constructor of OutputStructure is set zero

    area_vec.resize(NUM_AREA);
    for (int i = 0; i < NUM_AREA; ++i) {       // precompile matrix for convert from polar to cartesian
        double angle = -static_cast<double >(i) / static_cast<double>(NUM_AREA) * 360.;
        area_vec[i] = Area(AREA_SIZE, AREA_SIZE, -angle, angle, AREA_DISTANCE_METERS);
    }

    AREA_SIZE = AREA_SIZE_DEF;
    AREA_DISTANCE_METERS = AREA_DISTANCE_METERS_DEF;
    K_MAX = K_MAX_DEF;
}

void MainProcess::remesh(const double step) {

    AREA_SIZE = AREA_SIZE_DEF * (step / STEP_DEF);
    AREA_DISTANCE_METERS = AREA_DISTANCE_METERS_DEF * (step / STEP_DEF);
    K_MAX = 2. * M_PI / static_cast<double>(AREA_SIZE) * static_cast<double>(CUT_NUM);

    for (int i = 0; i < NUM_AREA; ++i) {       // precompile matrix for convert from polar to cartesian
        double angle = -static_cast<double >(i) / static_cast<double>(NUM_AREA) * 360.;

        // TODO: see if this ok for longer steps
        area_vec[i] = Area(AREA_SIZE, AREA_SIZE, -angle, angle, AREA_DISTANCE_METERS);
    }

    std::cout << "You changed STEP!!! Remeshed successfully!!!" << std::endl;
}

void MainProcess::run_realtime() {

    index = 0;
    std::queue<InputStructure> data_queue;     // queue of last not more MEAN results
    std::mutex mtx;                            // mutex for changing queue when input processor want to change it

    std::cout << "start run_realtime" << std::endl;

    std::thread reader_thread([&]() { // thread for reading data and convert it to InputStructure

        while (true) {                          // infinity loop

            try {
                const auto start = std::chrono::steady_clock::now();

                InputStructure inp = inp_proc->listen_message();

                const auto end = std::chrono::steady_clock::now();
                const std::chrono::duration<double> elapsed_seconds = end - start;

                std::cout << "num shot " << index << ", shots in queue " << data_queue.size() << ", read time "
                          << elapsed_seconds.count() << "s" << std::endl;

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (data_queue.size() >= MEAN) data_queue.pop(); // if queue overflowed delete the oldest element
                    data_queue.push(inp);                         // add new element
                }

            } catch (const SeaVisionException &exception) {

                std::cerr << "Global read error: " << exception.what() << std::endl;
                continue;
            }
        }
    });

    std::thread processor_thread([&]() {

        while (true) {

            try {

                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(TURN_PERIOD * 10)));

                InputStructure local_copy;               // copy of the oldest element in queue
                bool change = false;                     // flag if processor works speedier than sender

                {
                    std::lock_guard<std::mutex> lock(mtx); // lock if input processor want to change queue
                    if (!data_queue.empty()) {           // queue must be not empty for next processing
                        local_copy = data_queue.front(); // copy the oldest element
                        data_queue.pop();                // remove it from queue
                        change = true;                   // we detected changes, we can process next
                    }
                }

                if (change) {

                    const auto start = std::chrono::steady_clock::now();
                    update(local_copy); // process data
                    const auto end = std::chrono::steady_clock::now();
                    const std::chrono::duration<double> elapsed_seconds = end - start;

                    std::cout << "num shot " << index << ", shots in queue " << data_queue.size() << ", update time "
                              << elapsed_seconds.count() << "s" << std::endl;

                    OutputStructure res = get_mean_output(); // prepare results for sending
                    output_proc->pass_message(res); // sending

                    std::cout << "pass result: dir " << res.dir[0] << ", swh " << res.swh[0] << ", vcos " << res.vcos[0]
                              << ", per " << res.per[0] << ", len " << res.len[0] << std::endl << std::endl;
                }

            } catch (const SeaVisionException &exception) {
                std::cerr << "Process error: " << exception.what() << std::endl;
                continue;
            }
        }
    });

    reader_thread.join();
    processor_thread.join();
}

void MainProcess::update(const InputStructure &input) {

    if (index % 16 == 0) {

        std::ofstream out("C:/ocean/SEAVISION_CPP/back" + std::to_string(index) + ".csv");

        for (int i = 0; i < input.prli.bcksctr.rows(); ++i) {
            for (int j = 0; j < input.prli.bcksctr.cols(); ++j) {
                out << input.prli.bcksctr(i, j) << ",";
            }
            out << std::endl;
        }
    }

    if (curr_step != input.prli.step) {
        curr_step = input.prli.step;
        remesh(curr_step);
    }

    speed[index % MEAN] = input.navi.spd;               // update current vessel speed
    hgd[index % MEAN] = input.navi.hdg;                 // update current vessel heading

    // find NUM_SYSTEMS indexes of zones with the most contrast signal
    Eigen::VectorXi new_ang = disp_direct->calc_directions(input.prli.bcksctr, index, curr_step);

    dir_vec[index % CHANGE_DIR_NUM_SHOTS] = new_ang[0]; // update main direction where after we do Fourier transform

    if (index < MEAN) curr_dir = new_ang[0];
    else if (index == MEAN) curr_dir = get_median(dir_vec, false);
    else {
        if (index % CHANGE_DIR_NUM_SHOTS == 0) curr_dir = get_median(dir_vec, true);
    }

    auto inp_back = mesh->calc_back(area_vec[curr_dir], input.prli.bcksctr, curr_step);

    curve->update(index, inp_back);

    index++;
    if (index >= 3 * FOUR_NUM) index = 2 * FOUR_NUM; // avoid overflowing

    mean_output[index % MEAN] = make_output();
}

OutputStructure MainProcess::make_output() {

    OutputStructure res;
    res.rose = disp_direct->get_rose();
    res.len = disp_direct->get_len();
    res.dir = disp_direct->get_dirs();

    if (!four_or_disp) res.swh = disp_direct->get_swh();

    WaveStruct spec = curve->get_params();
    res.freq_spec = spec.freq_spec;

    for (int i = 0; i < NUM_SYSTEMS; i++) {

        bool cond = ((spec.m0[i] <= 1e-3) and (std::abs(spec.vcosalpha[i]) <= 1e-6))
                    or ((spec.m0[i] <= 2e-3) and (std::abs(spec.vcosalpha[i]) > 2.)); // / speed.mean()

        if (cond) {
            res.per[i] = 0.;
            res.m0[i] = 0.;
            res.vcos[i] = 0.;

            if (four_or_disp) res.swh[i] = 0.;

            if (index >= FOUR_NUM) {
                res.len[i] = 0.;
                res.dir[i] = 0.;
                res.swh[i] = 0.;
            }

        } else {
            res.per[i] = spec.peak_period[i];

            // TODO: here we can use different coefficients for steps
            if (four_or_disp) res.swh[i] = A_COEFF + B_COEFF * std::sqrt(spec.m0[i]);
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

    double size = MEAN;
    if (index < MEAN) size = index;
    else if (index > FOUR_NUM && index < FOUR_NUM + MEAN) size = index - FOUR_NUM;

    for (int j = 0; j < size; ++j) {
        res.freq_spec += mean_output[j].freq_spec / size;
        res.rose += mean_output[j].rose / size;
        for (int i = 0; i < NUM_SYSTEMS; ++i) {
            res.swh[i] += mean_output[j].swh[i] / size;
            res.per[i] += mean_output[j].per[i] / size;
            res.dir[i] += mean_output[j].dir[i] / size;
            res.len[i] += mean_output[j].len[i] / size;
            res.m0[i] += mean_output[j].m0[i] / size;
            res.vcos[i] += mean_output[j].vcos[i] / size;
        }
    }

    return res;
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

} // namespace