#include "pixelfuncs.hpp"

void erosion(cv::Mat &img_in, cv::Mat &img_out, bool b8) {
    // consider taking the SE as an input
    cv::Point2i center(0,0), up(0,-1), down(0,1), left(-1,0), right(1,0);
    std::vector<cv::Point2i> se = {center, up, down, left, right};
    if(b8) {
        se.push_back(up + left);
        se.push_back(up + right);
        se.push_back(down + left);
        se.push_back(down + right);
    }

    cv::MatConstIterator_<uchar> iter_end = img_in.end<uchar>();
    cv::MatConstIterator_<uchar> iter = img_in.begin<uchar>();
    cv::Point2i n;
    for(; iter != iter_end; iter++) {
        img_out.at<uchar>(iter.pos()) = 255;
        for (int i = 0; i < se.size(); i++) {
            n = iter.pos() + se[i];
            if (n.x < 0 || n.y < 0) {
                    continue;
            }
            if (img_in.at<uchar>(n) != 255) {
                img_out.at<uchar>(iter.pos()) = 0;
                break;
            }
        }
    }
}

void dilation(cv::Mat &img_in, cv::Mat &img_out, bool b8) {
    // consider taking the SE as an input
    cv::Point2i center(0,0), up(0,-1), down(0,1), left(-1,0), right(1,0);
    std::vector<cv::Point2i> se = {center, up, down, left, right};
    if(b8) {
        se.push_back(up + left);
        se.push_back(up + right);
        se.push_back(down + left);
        se.push_back(down + right);
    }

    cv::MatConstIterator_<uchar> iter_end = img_in.end<uchar>();
    cv::MatConstIterator_<uchar> iter = img_in.begin<uchar>();
    cv::Point2i n;
    for(; iter != iter_end; iter++) {
        img_out.at<uchar>(iter.pos()) = 0;
        for (int i = 0; i < se.size(); i++) {
            n = iter.pos() + se[i];
            if (n.x < 0 || n.y < 0) {
                    continue;
            }
            if (img_in.at<uchar>(n) == 255) {
                img_out.at<uchar>(iter.pos()) = 255;
                break;
            }
        }
    }
}