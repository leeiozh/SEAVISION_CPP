//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_PICTURE_HPP
#define POLYGON_PICTURE_HPP

#include "../Matrix/Matrix.hpp"
#include "../PSF/PSF.hpp"
#include "../PSF/AtmosphericNoise/BaseAtmosphericNoise.hpp"
#include "SkyLight.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class Picture {
protected:
    Matrix<SIZE_X, SIZE_Y> matrix;
    PSF<SIZE_X, SIZE_Y> psf;
    std::shared_ptr<BaseVisualField> field;
    SkyLight<SIZE_X, SIZE_Y> sky_light;

public:
    inline Picture(const Matrix<SIZE_X, SIZE_Y> &matrix, const PSF<SIZE_X, SIZE_Y> &psf,
                   std::shared_ptr<BaseVisualField> field, const SkyLight<SIZE_X, SIZE_Y> &sky_light) :
            matrix(matrix), psf(psf), field(std::move(field)), sky_light(sky_light) {}

    inline void bilinear_interpol(Eigen::MatrixXd &res, const Eigen::Vector2d &pos, const double &bright) {
        double x = 0.5 * (SIZE_X - 1) * (pos.x() + 1);
        double y = 0.5 * (SIZE_Y - 1) * (pos.y() + 1);
        int x_div = floor(x);
        double x_mod = x - x_div;
        int y_div = floor(y);
        double y_mod = y - y_div;

        if (((x_div + 1) < SIZE_X) && ((y_div + 1) < SIZE_Y)) {
            res(x_div, y_div) = bright * (1 - x_mod) * (1 - y_mod);
            res(x_div + 1, y_div) = bright * x_mod * (1 - y_mod);
            res(x_div, y_div + 1) = bright * (1 - x_mod) * y_mod;
            res(x_div + 1, y_div + 1) = bright * x_mod * y_mod;
        }
    }

    [[nodiscard]] inline Eigen::MatrixXd calc_roll(const Eigen::MatrixXd &pic, const Eigen::MatrixXd &h_matrix) const {
        fftw_complex *f_pic;
        f_pic = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        fftw_plan p1 = fftw_plan_dft_2d(SIZE_X, SIZE_Y, convert_eigen_to_fftw<SIZE_X, SIZE_Y>(pic), f_pic, FFTW_FORWARD,
                                        FFTW_ESTIMATE);
        fftw_execute(p1);
        fftw_destroy_plan(p1);
        fftw_cleanup();

        fftw_complex *f_h;
        f_h = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        fftw_plan p2 = fftw_plan_dft_2d(SIZE_X, SIZE_Y, convert_eigen_to_fftw<SIZE_X, SIZE_Y>(h_matrix), f_h,
                                        FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(p2);
        fftw_destroy_plan(p2);
        fftw_cleanup();

        fftw_complex *f_mult;
        f_mult = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        for (int i = 0; i < SIZE_X; ++i) {
            for (int j = 0; j < SIZE_Y; ++j) {
                f_mult[i * SIZE_X + j][0] = f_pic[i * SIZE_X + j][0] * f_h[i * SIZE_X + j][0] -
                                            f_pic[i * SIZE_X + j][1] * f_h[i * SIZE_X + j][1];
                f_mult[i * SIZE_X + j][1] = f_pic[i * SIZE_X + j][0] * f_h[i * SIZE_X + j][1] +
                                            f_pic[i * SIZE_X + j][1] * f_h[i * SIZE_X + j][0];
            }
        }

        fftw_complex *res;
        res = (fftw_complex *) malloc(SIZE_X * SIZE_Y * sizeof(fftw_complex));

        fftw_plan p3 = fftw_plan_dft_2d(SIZE_X, SIZE_Y, f_mult, res, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(p3);
        fftw_destroy_plan(p3);
        fftw_cleanup();

        return convert_fftw_to_eigen<SIZE_X, SIZE_Y>(res) / 1e17;
    }

    inline std::array<Eigen::MatrixXi, COLOR_NUMBER>
    calc_pictures(const ScopeState &scope_state, std::vector<SatState> &sat_state,
                  const Eigen::Vector3d &sun_pos, const double &time_expose) {
        std::array<Eigen::MatrixXd, COLOR_NUMBER> res;

        auto star_array = field->view_star_array(scope_state.direction);
        auto sat_array = field->check_sat_array(scope_state, sat_state, sun_pos);

        for (int i = 0; i < COLOR_NUMBER; ++i) {
            Eigen::MatrixXd pic = Eigen::MatrixXd::Zero(SIZE_X, SIZE_Y);

            for (const auto &star: star_array) {
                bilinear_interpol(pic, star.position2d, star.bright);
            }

            for (const auto &sat: sat_array) {
                bilinear_interpol(pic, sat.position2d, sat.bright);

            }
            pic += sky_light.calc_background(scope_state, field);
            res[i] = calc_roll(pic, psf.get_matrix());
        }

        matrix.set_pictures(res);

        return matrix.to_electrons_with_noise(time_expose);

    };

};

}

#endif //POLYGON_PICTURE_HPP
