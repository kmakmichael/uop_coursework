#include "kernels.hpp"

cv::Mat1f gaussian(float sigma) {
    float a = floor(2.5 * sigma);
    // printf("preparing kernel of length %d\n", 2*a+1);
    cv::Mat1f kern(1, 2*a+1, CV_32FC1);
    float sum = 0;
    for (auto i = kern.begin(); i != kern.end(); i++) {
        float x = static_cast<float>(i.pos().x);
        *i = exp((-1.0 * (x-a) * (x-a)) / (2.0 * sigma * sigma));
        sum += *i;
    }
    for (auto i = kern.begin(); i != kern.end(); i++) {
        *i /= sum;
    }
    return kern;
}

cv::Mat1f deriv(float sigma) {
    float a = floor(2.5 * sigma);
    // printf("preparing deriv kernel of length %d\n", 2*a+1);
    cv::Mat1f kern(1, 2*a+1, CV_32FC1);
    float sum = 0;
    for (auto i = kern.begin(); i != kern.end(); i++) {
        float x = static_cast<float>(i.pos().x);
        *i = -1.0 * (x-a) * exp((-1.0 * (x-a) * (x-a)) / (2.0 * sigma * sigma));
        sum -= *i * x;
    }
    for (auto i = kern.begin(); i != kern.end(); i++) {
        *i /= sum;
    }
    // kernel flipping
    for (int i = 0; i < (kern.cols/2); i++) {
        float temp = kern.at<float>(0,kern.cols-1-i);
        kern.at<float>(0,kern.cols-1-i) = kern.at<float>(0,i);
        kern.at<float>(0,i) = temp;
    }
    kern.at<float>(0,(int)a) = 0.0; // make it positive zero, not -0
    return kern;
}

void print_kern(cv::Mat1f kern) {
    printf("Kernel(%d,%d): \n[ ", kern.cols, kern.rows);
    for (int r = 0; r < kern.rows; r++) {
        for (int c = 0; c < kern.cols; c++) {
            printf("%f ", kern.at<float>(r,c));
        }
        printf("\n");
    }
    printf("]\n");
}