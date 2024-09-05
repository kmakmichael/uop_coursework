#ifndef PIXELFUNCS_H
#define PIXELFUNCS_H

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <cstdint>

/*
    performs histogram equalization on an image
 
    notes:
        could benefit from use of the Mat.setTo() function. Changing
        an array of pixels in parallel might be worth the overhead of
        storing a matrix of pixels with that value (pdf would be as 
        big as the original image at minimum to store all the indices)
 */
void histeq(cv::Mat &img);

/*
    Floodfill
*/
void floodfill(cv::Mat &img, cv::Point2i &seed, uchar fill_color);
void floodfill(cv::Mat &img_in, cv::Mat &img_out, cv::Point2i &seed, uchar fill_color);

/*
    Thresholding
*/
unsigned char ridler_calvard(cv::Mat &img);
void simple_thresh(cv::Mat &img, unsigned char t);
void double_thresh(cv::Mat &img, cv::Mat &d_thr);

/*
    Morphological operations
*/
void erosion(cv::Mat &img, cv::Mat &img_out, bool b8);
void dilation(cv::Mat &img, cv::Mat &img_out, bool b8);

#endif