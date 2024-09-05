#ifndef KERNELS_H
#define KERNELS_H

#include <opencv2/opencv.hpp>
#include <cmath>

cv::Mat1f gaussian(float sigma);
cv::Mat1f deriv(float sigma);
void print_kern(cv::Mat1f kern);

#endif