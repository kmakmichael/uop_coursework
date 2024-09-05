#include "proc.hpp"

bool boundary_check(cv::Mat1f img, cv::Point pt) {
    if (pt.x >= 0 && pt.x < img.cols) {
        if (pt.y >= 0 && pt.y < img.rows) {
            return true;
        }
    }
    return false;
}


cv::Mat1f suppress(cv::Mat1f direction, cv::Mat1f magnitude) {
    cv::Mat1f supp = cv::Mat::zeros(direction.rows, direction.cols, CV_32FC1);
    float theta = 0;
    cv::Point2i up(0,-1), down(0,1), left(-1,0), right(0,-1);
    for (auto s = supp.begin(); s != supp.end(); s++) {
        theta = direction.at<float>(s.pos());
        if (theta < 0) {
            theta += M_PI;
        }
        theta *= (180.0 / M_PI);
        *s = magnitude.at<float>(s.pos());
        if (theta <= 22.5 || theta > 157.5) {
            // top
            if (s.pos().y > 0) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + up)) {
                    *s = 0;
                }
            }
            // bottom
            if (s.pos().y < supp.rows-1) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + down)) {
                    *s = 0;
                }
            }
        } else if (theta > 22.5 && theta <= 67.5) {
            //topleft
            if (s.pos().y > 0 && s.pos().x > 0) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + up + left)) {
                    *s = 0;
                }
            }
            // bottomright
            if (s.pos().y < supp.rows-1 && s.pos().x < supp.cols-1) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + down + right)) {
                    *s = 0;
                }
            }
        } else if (theta > 67.5 && theta <= 112.5) {
            // left
            if (s.pos().x > 0) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + left)) {
                    *s = 0;
                }
            }
            // right
            if (s.pos().x < supp.cols-1) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + right)) {
                    *s = 0;
                }
            }
        } else if (theta > 112.5 && theta <= 157.5) {
            // topright
            if (s.pos().y > 0 && s.pos().x < supp.cols-1) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + up + right)) {
                    *s = 0;
                }
            }
            // bottomleft
            if (s.pos().y < supp.rows-1 && s.pos().x > 0) {
                if (magnitude.at<float>(s.pos()) < magnitude.at<float>(s.pos() + down + left)) {
                    *s = 0;
                }
            }
        }
    }
    return supp;
}

void hysteresis(cv::Mat1f img, float t_high, float t_low) {
    for(auto i = img.begin(); i != img.end(); i++) {
        if (*i >= t_high) {
            *i = 255;
        } else if (*i <= t_low) {
            *i = 0;
        } else {
            *i = 125;
        }
    }
}


cv::Mat1f edge_linking(cv::Mat1f hyst) {
    cv::Mat1f edges = cv::Mat::zeros(hyst.rows, hyst.cols, CV_32FC1);
    cv::Point2i p, opt(-1,-1);
    cv::Mat1f omat = cv::Mat::zeros(3,3,CV_32FC1);
    auto h = hyst.begin();
    auto e = edges.begin();
    for (; e != edges.end(); h++, e++) {
        if(*h == 125) {
            *e = 0;
            for (auto oit = omat.begin(); oit != omat.end(); oit++) {
                p = e.pos() + (oit.pos() - opt);
                if (p.x >= 0 && p.y >= 0 && p.x < edges.cols && p.y < edges.rows) {
                    if (hyst.at<float>(p) == 255) {
                        *e = 255;
                    }
                }
            }
        } else {
            *e = *h;
        }
    }
    return edges;
}