#include "pixelfuncs.hpp"

void floodfill(cv::Mat &img_in, cv::Point2i &seed, uchar fill_color) {
    std::vector<cv::Point2i> frontier;

    uchar old_color = img_in.at<uchar>(seed);
    frontier.push_back(seed);
    img_in.at<uchar>(seed) = fill_color;
    while (frontier.size() != 0) {
        cv::Point2i q = frontier.back();
        frontier.pop_back();
        for (int r = -1; r < 2; r++) {
            for (int c = -1; c < 2; c++) {
                cv::Point2i n = q + cv::Point2i(r, c);
                if (n.x < 0 || n.y < 0) {
                    continue;
                }
                if (img_in.at<uchar>(n) == old_color) {
                    frontier.push_back(n);
                    img_in.at<uchar>(n) = fill_color;
                }
            }
        }
    }
}

void floodfill(cv::Mat &img_in, cv::Mat &img_out, cv::Point2i &seed, uchar fill_color) {
    std::vector<cv::Point2i> frontier;
    uchar old_color = img_in.at<uchar>(seed);
    frontier.push_back(seed);
    img_out.at<uchar>(seed) = fill_color;
    while (frontier.size() != 0) {
        cv::Point2i q = frontier.back();
        frontier.pop_back();
        for (int r = -1; r < 2; r++) {
            for (int c = -1; c < 2; c++) {
                cv::Point2i n = q + cv::Point2i(r, c);
                if (n.x < 0 || n.y < 0) {
                    continue;
                }
                if (img_in.at<uchar>(n) == old_color && img_out.at<uchar>(n) != fill_color) {
                    frontier.push_back(n);
                    img_out.at<uchar>(n) = fill_color;
                }
            }
        }
    }
}