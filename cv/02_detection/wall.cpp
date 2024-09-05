#include "wall.hpp"


// helpers
enum Direction {up=0, right=1, down=2, left=3};
cv::Point2i find_start(cv::Mat &img, uchar color);
cv::Point2i dir2pt(Direction d);
cv::Point2i dir2pt(int i);


#define TURN_RIGHT(x) x = static_cast<Direction>((x+1)%4)
#define TURN_LEFT(x) x = static_cast<Direction>((x+3)%4)
#define R_RIGHT(x) (x+1)%4 // relative right
#define R_LEFT(x) (x+3)%4 // relative left
#define FRONT(x) img.at<uchar>(compass(x.dir))
cv::Mat wall(cv::Mat &img, uchar color) {
    cv::Mat mask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
    cv::Point2i start = find_start(img, color);
    cv::Point2i pos = start;
    Direction dir = up;
    while (img.at<uchar>(pos + dir2pt(dir)) == color) {
        TURN_RIGHT(dir);
    }
    TURN_RIGHT(dir);
    while (mask.at<uchar>(start) != 255) {
        if (img.at<uchar>(pos + dir2pt(R_LEFT(dir))) == color) {
            TURN_LEFT(dir);
            pos = pos + dir2pt(dir);
            mask.at<uchar>(pos) = 255;
        } else if (img.at<uchar>(pos + dir2pt(dir)) != color) {
            TURN_RIGHT(dir);
        } else {
            pos = pos + dir2pt(dir);
            mask.at<uchar>(pos) = 255;
        }
    }
    return mask;
}
#undef TURN_RIGHT
#undef TURN_LEFT
#undef R_RIGHT
#undef R_LEFT
#undef FRONT


cv::Point2i find_start(cv::Mat &img, uchar color) {
    cv::MatConstIterator_<uchar> iter_end = img.end<uchar>();
    cv::MatIterator_<uchar> iter = img.begin<uchar>();
    for(; iter != iter_end; iter++) {
        if (*iter == color) {
            return iter.pos();
        }
    }
}


cv::Point2i dir2pt(Direction d) {
    switch (d) {
        case up:    return cv::Point2i(0,-1);
        case right: return cv::Point2i(1,0);
        case down:  return cv::Point2i(0,1);
        case left:  return cv::Point2i(-1,0);
    }
}


cv::Point2i dir2pt(int i) {
    switch (static_cast<Direction>(i)) {
        case up:    return cv::Point2i(0,-1);
        case right: return cv::Point2i(1,0);
        case down:  return cv::Point2i(0,1);
        case left:  return cv::Point2i(-1,0);
    }
}