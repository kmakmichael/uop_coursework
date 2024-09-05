#ifndef PCA_H
#define PCA_H

#include <utility>
#include <cmath>
#include <opencv2/core/mat.hpp>

#include "moment.hpp"
#include "region.hpp"


std::pair<double, double> eigen(cv::Mat &img, uchar color);
std::pair<double, double> eigen(cv::SparseMat &u);
std::pair<double, double> sm_ax_len(std::pair<double, double> evals);
double direction(cv::SparseMat &u);
double eccentricity(cv::SparseMat &u);

#endif