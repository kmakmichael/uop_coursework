#include "pixelfuncs.hpp"

#include <opencv2/opencv.hpp>

unsigned char ridler_calvard(cv::Mat &img) {
    uint32_t pdf[256] = {};
    // calculate pdf
    cv::MatConstIterator_<uchar> iter_end = img.end<uchar>();
    cv::MatIterator_<uchar> iter = img.begin<uchar>();
    for(; iter != iter_end; iter++) {
        pdf[*iter]++;
    }

    // prep m_0 & m_1
    uint32_t m_0[256] = {};
    uint32_t m_1[256] = {};
    m_0[0] = pdf[0];
    m_1[0] = 1;
    for(size_t i = 1; i < 256; i++) {
        m_0[i] = m_0[i-1] + pdf[i];
        m_1[i] = m_1[i-1] + pdf[i]*i;
    }

    float t = 127, t_old = 0;
    float u_bg, u_fg;
    for (size_t i = 0; i < 100; i++) {
        t_old = t;
        u_bg = m_1[(size_t) t] / (float) m_0[(size_t)t];
        u_fg = (m_1[255] - m_1[(size_t)t]) / (float)(m_0[255] - m_0[(size_t)t]);
        t = (u_fg + u_bg) / 2.0;
        if (t - t_old < 0.1 && t - t_old > -0.1) {
            return t;
        }
    }
    return t;
}

void simple_thresh(cv::Mat &img, unsigned char t) {
    cv::MatConstIterator_<uchar> iter_end = img.end<uchar>();
    cv::MatIterator_<uchar> iter = img.begin<uchar>();
    for(; iter != iter_end; iter++) {
        if (*iter <= t) {
            *iter = (unsigned char) 0;
        } else {
            *iter = (unsigned char) 255;
        }
    }
}

void double_thresh(cv::Mat &img, cv::Mat &d_thr) {
    unsigned char t = ridler_calvard(img);
    #define t_range 0.85
    unsigned char t_lo = t;
    unsigned char t_hi = (1/t_range) * t;
    cv::Mat img_lo = img.clone();
    cv::Mat img_hi = img.clone();
    simple_thresh(img_lo, t_lo);
    simple_thresh(img_hi, t_hi);

    // floodfills
    cv::MatConstIterator_<uchar> iter_end = img_hi.end<uchar>();
    cv::MatIterator_<uchar> iter = img_hi.begin<uchar>();
    cv::Point2i q;
    for(; iter != iter_end; iter++) {
        if (*iter == 255) {
            q = iter.pos();
            floodfill(img_lo, d_thr, q, 255);    
        }
    }
}