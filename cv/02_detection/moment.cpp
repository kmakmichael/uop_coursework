#include "moment.hpp"


// helper functions
double moment_x(cv::Mat &region, uchar color, uint8_t p);
double moment_y(cv::Mat &region, uchar color, uint8_t q);
double moment_00(cv::Mat &region, uchar color);

double moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q) {
    if (!p && !q) {
        return moment_00(region, color);
    }
    if (!p) {
        return moment_y(region, color, q);
    } 
    if (!q) {
        return moment_x(region, color, p);
    }
    double sum = 0;
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += pow(iter.pos().x, p) * pow(iter.pos().y, q);
        }
    }
    return sum;
}

double moment_x(cv::Mat &region, uchar color, uint8_t p) {
    double sum = 0;
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += pow(iter.pos().x, p);
        }
    }
    return sum;
}

double moment_y(cv::Mat &region, uchar color, uint8_t q) {
    double sum = 0;
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += pow(iter.pos().y, q);
        }
    }
    return sum;
}

double moment_00(cv::Mat &region, uchar color) {
    double sum = 0;
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += 1;
        }
    }
    return sum;
}

double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q) {
    if (!p && !q) {
        return moment_00(region, color);
    }
    double sum = 0;
    double m_00 = moment(region, color, 0, 0);
    double m_01 = moment(region, color, 0, 1);
    double m_10 = moment(region, color, 1, 0);
    cv::Point2f cen(m_10/m_00, m_01/m_00);
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += pow(iter.pos().x - cen.x, p) * pow(iter.pos().y - cen.y, q);
        }
    }
    return sum;
}


double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q, cv::Point2f cen) {
    if (!p && !q) {
        return moment_00(region, color);
    }
    double sum = 0;
    cv::MatConstIterator_<uchar> iter = region.begin<uchar>();
    for(; iter != region.end<uchar>(); iter++) {
        if (*iter == color) {
            sum += pow(iter.pos().x - cen.x, p) * pow(iter.pos().y - cen.y, q);
        }
    }
    return sum;
}