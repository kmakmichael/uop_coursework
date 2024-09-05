#include "pca.hpp"


std::pair<double, double> eigen(cv::Mat &img, uchar color) {
    double e1, e2;
    double u_00 = central_moment(img, color, 0, 0);
    double u_20 = central_moment(img, color, 2, 0);
    double u_02 = central_moment(img, color, 0, 2);
    double u_11 = central_moment(img, color, 1, 1);

    // printf("trying to root %.2f\n", pow(u_20-u_02, 2) - pow(2*u_11, 2));
    double sq = sqrt(pow(u_20-u_02, 2) - pow(2*u_11, 2));
    printf("sq = %.2f\n", sq);
    e1 = (u_20 + u_02 + sq) / (2*u_00);
    e2 = (u_20 + u_02 - sq) / (2*u_00);
    return std::make_pair(e1, e2);
}


std::pair<double, double> sm_ax_len(std::pair<double, double> evals) {
    return std::make_pair(2*sqrt(evals.first), 2*sqrt(evals.second));
}


/*
    Takes a sparse matrix of moments instead of calculating its own
*/
#define U(x, y) u.value<double>(x, y)
std::pair<double, double> eigen(cv::SparseMat &u) {
    double sq = sqrt(pow(U(2,0)-U(0,2), 2) + pow(U(1,1), 2));
    double e1 = (U(2,0) + U(0,2) + sq) / (2*U(0,0));
    double e2 = (U(2,0) + U(0,2) - sq) / (2*U(0,0));
    return std::make_pair(e1, e2);
}


double direction(cv::SparseMat &u) {
    return 0.5 * atan2(2.0 * U(1,1), U(2,0) - U(0,2));
}


double eccentricity(cv::SparseMat &u) {
    double sq = sqrt(pow(U(2,0)-U(0,2), 2) + pow(2*U(1,1), 2));
    return sqrt((2*sq) / (U(2,0)+U(0,2)+sq));
}
#undef U