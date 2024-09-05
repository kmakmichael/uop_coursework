#ifndef REGION_H
#define REGION_H

#include <utility>
#include <cmath>
#include <opencv2/core/mat.hpp>

#include "moment.hpp"
#include "pca.hpp"

enum Fruit{mystery_fruit, banana, orange, apple};
typedef struct {
    uchar color;
    cv::Point2i centroid;
    cv::SparseMat m;
    cv::SparseMat u;
    std::pair<double, double> eigen;
    double dir;
    double ecc;
    Fruit f;
} region;


std::vector<region> image_analysis(cv::Mat &img);
region analyze_region(cv::Mat &img, uchar color);
cv::SparseMat moments(cv::Mat &region, uchar color);
cv::SparseMat central_moments(cv::Mat &region, uchar color);
cv::SparseMat central_moments(cv::Mat &region, uchar color, cv::Point2f cen);
void print_region_info(region r);

#endif