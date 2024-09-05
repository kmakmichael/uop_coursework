#ifndef PROC_H
#define PROC_H

#include <opencv2/opencv.hpp>
#include <math.h>

cv::Mat1f suppress(cv::Mat1f direction, cv::Mat1f magnitude);
void hysteresis(cv::Mat1f img, float t_high, float t_low);
cv::Mat1f edge_linking(cv::Mat1f hyst);

#endif