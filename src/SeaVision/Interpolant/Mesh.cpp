//
// Created by leeiozh on 13.04.23.
//

#include <iostream>
#include "Mesh.hpp"

namespace SeaVision {

Mesh::Mesh(const int &size_dist, const int &size_angle, const double &dist_step)
        : size_dist(size_dist), size_angle(size_angle), dist_step(dist_step) {

    int size_mesh = static_cast<int>(2 * size_dist); // pixels
    mesh.resize(size_mesh, size_mesh);
    for (int i = 0; i < size_mesh; ++i) { // pixels
        double ii = (static_cast<double>(i) / size_mesh - 0.5) * 2 * size_dist; // in pixels
        for (int j = 0; j < size_mesh; ++j) {
            double jj = (static_cast<double>(j) / size_mesh - 0.5) * 2 * size_dist; // in pixels
            double rho = std::sqrt(ii * ii + jj * jj); // meters
            int rho_div = std::floor(rho);
            double theta = (-std::atan2(ii, jj) / 2. / M_PI + 0.25) * size_angle; // in pixels
            if (theta < 0) {
                theta += size_angle;
            }
            int theta_div = std::floor(theta);

            mesh(i, j) = Cell{{rho_div,       theta_div},
                              {rho - rho_div, theta - theta_div}};
        }
    }
}

Eigen::MatrixXd Mesh::calc_back(const SeaVision::Area &area, const Eigen::MatrixXi &back) const {

    Eigen::Matrix<double, 4, 2> vertexes = area.getCoordVertex() / dist_step; // pixels

    int res_size_x = static_cast<int>(area.size_x / dist_step);
    int res_size_y = static_cast<int>(area.size_y / dist_step);

    Eigen::MatrixXd res = Eigen::MatrixXd::Zero(res_size_x, res_size_y);

    for (int i = 0; i < res_size_x; ++i) {
        double y_norm = static_cast<double>(i) / res_size_y;
        double ii = vertexes(2, 0) + y_norm * (vertexes(1, 0) - vertexes(2, 0));
        double jj = vertexes(2, 1) + y_norm * (vertexes(1, 1) - vertexes(2, 1));
        for (int j = 0; j < res_size_y; ++j) {
            double x_norm = static_cast<double >(j) / res_size_x;
            double x = size_dist + ii + x_norm * (vertexes(3, 0) + y_norm * (vertexes(0, 0) - vertexes(3, 0)) - ii);
            int x_div = static_cast<int>(x);
            double y = size_dist + jj + x_norm * (vertexes(3, 1) + y_norm * (vertexes(0, 1) - vertexes(3, 1)) - jj);
            int y_div = static_cast<int>(y);

            res(i, j) =
                    (1 - (x - x_div)) * (1 - (y - y_div)) *
                    (back(mesh(x_div, y_div).indexes[0], mesh(x_div, y_div).indexes[1]) *
                     (1 - mesh(x_div, y_div).weights[0]) * (1 - mesh(x_div, y_div).weights[1]) +
                     back(mesh(x_div, y_div).indexes[0] + 1, mesh(x_div, y_div).indexes[1]) *
                     mesh(x_div, y_div).weights[0] * (1 - mesh(x_div, y_div).weights[1]) +
                     back(mesh(x_div, y_div).indexes[0], mesh(x_div, y_div).indexes[1] + 1) *
                     (1 - mesh(x_div, y_div).weights[0]) * mesh(x_div, y_div).weights[1] +
                     back(mesh(x_div, y_div).indexes[0] + 1, mesh(x_div, y_div).indexes[1] + 1) *
                     mesh(x_div, y_div).weights[0] * mesh(x_div, y_div).weights[1]) +
                    (x - x_div) * (1 - (y - y_div)) *
                    (back(mesh(x_div + 1, y_div).indexes[0], mesh(x_div + 1, y_div).indexes[1]) *
                     (1 - mesh(x_div + 1, y_div).weights[0]) * (1 - mesh(x_div + 1, y_div).weights[1]) +
                     back(mesh(x_div + 1, y_div).indexes[0] + 1, mesh(x_div + 1, y_div).indexes[1]) *
                     mesh(x_div + 1, y_div).weights[0] * (1 - mesh(x_div + 1, y_div).weights[1]) +
                     back(mesh(x_div + 1, y_div).indexes[0], mesh(x_div + 1, y_div).indexes[1] + 1) *
                     (1 - mesh(x_div + 1, y_div).weights[0]) * mesh(x_div + 1, y_div).weights[1] +
                     back(mesh(x_div + 1, y_div).indexes[0] + 1, mesh(x_div + 1, y_div).indexes[1] + 1) *
                     mesh(x_div + 1, y_div).weights[0] * mesh(x_div + 1, y_div).weights[1]) +
                    (1 - (x - x_div)) * (y - y_div) *
                    (back(mesh(x_div, y_div + 1).indexes[0], mesh(x_div, y_div + 1).indexes[1]) *
                     (1 - mesh(x_div, y_div + 1).weights[0]) * (1 - mesh(x_div, y_div + 1).weights[1]) +
                     back(mesh(x_div, y_div + 1).indexes[0] + 1, mesh(x_div, y_div + 1).indexes[1]) *
                     mesh(x_div, y_div + 1).weights[0] * (1 - mesh(x_div, y_div + 1).weights[1]) +
                     back(mesh(x_div, y_div + 1).indexes[0], mesh(x_div, y_div + 1).indexes[1] + 1) *
                     (1 - mesh(x_div, y_div + 1).weights[0]) * mesh(x_div, y_div + 1).weights[1] +
                     back(mesh(x_div, y_div + 1).indexes[0] + 1, mesh(x_div, y_div + 1).indexes[1] + 1) *
                     mesh(x_div, y_div + 1).weights[0] * mesh(x_div, y_div + 1).weights[1]) +
                    (x - x_div) * (y - y_div) *
                    (back(mesh(x_div + 1, y_div + 1).indexes[0], mesh(x_div + 1, y_div + 1).indexes[1]) *
                     (1 - mesh(x_div + 1, y_div + 1).weights[0]) * (1 - mesh(x_div + 1, y_div + 1).weights[1]) +
                     back(mesh(x_div + 1, y_div + 1).indexes[0] + 1, mesh(x_div + 1, y_div + 1).indexes[1]) *
                     mesh(x_div + 1, y_div + 1).weights[0] * (1 - mesh(x_div + 1, y_div + 1).weights[1]) +
                     back(mesh(x_div + 1, y_div + 1).indexes[0], mesh(x_div + 1, y_div + 1).indexes[1] + 1) *
                     (1 - mesh(x_div + 1, y_div + 1).weights[0]) * mesh(x_div + 1, y_div + 1).weights[1] +
                     back(mesh(x_div + 1, y_div + 1).indexes[0] + 1, mesh(x_div + 1, y_div + 1).indexes[1] + 1) *
                     mesh(x_div + 1, y_div + 1).weights[0] * mesh(x_div + 1, y_div + 1).weights[1]);

//            std::cout << "1 " << i << " " << j << std::endl;
//            std::cout << "2 " << x << " " << y << std::endl;
//            std::cout << "3 " << mesh(x, y).indexes[0] << " " << mesh(x, y).indexes[1] << std::endl;
        }
    }
    return res;
}

}