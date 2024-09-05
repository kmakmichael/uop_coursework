#ifndef CONNCOM_H
#define CONNCOM_H

#include <opencv2/core/mat.hpp>
#include <cstdint>

size_t cc_floodfill(cv::Mat &input, cv::Mat &output);
size_t cc_union(cv::Mat &input, cv::Mat &output);

#endif