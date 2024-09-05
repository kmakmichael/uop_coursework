#ifndef MOMENT_H
#define MOMENT_H

#include <opencv2/opencv.hpp>
#include <cmath>

double moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q);
double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q);
double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q, cv::Point2f cen);


#endif