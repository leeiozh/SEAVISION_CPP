//
// Created by leeiozh on 01.09.22.
//

#ifndef POLYGON_PICTURE_HPP
#define POLYGON_PICTURE_HPP

#include "../Matrix/Matrix.hpp"
#include "../PSF/PSF.hpp"
#include "../VisualField/BaseVisualField.hpp"

namespace Polygon {

template <int SIZE_X, int SIZE_Y>

class Picture {
protected:
    Matrix<SIZE_X, SIZE_Y> matrix;
    PSF<SIZE_X, SIZE_Y> psf;
    std::shared_ptr<BaseVisualField> visualField;

public:
    Picture(const Matrix<SIZE_X, SIZE_Y> &matrix, const PSF<SIZE_X, SIZE_Y> &psf,
            const std::shared_ptr<BaseVisualField> &visualField) :
            matrix(matrix), psf(psf), visualField(visualField) {}

    std::array<Eigen::Matrix<double, SIZE_X, SIZE_Y>, COLOR_NUMBER> calc_pictures(){

    };

};

}

#endif //POLYGON_PICTURE_HPP
