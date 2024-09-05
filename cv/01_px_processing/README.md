# Project 1: Pixel Processing Functions

## Overview
This project implements the following pixel processing functions:
- histogram equalization
- floodfill
- thresholding
- dilation
- erosion

## Compile and Run:
To compile, make sure you have `cmake` installed:
```
cmake --build .
make
```
To run:
```
./px_processing <input image> <test>
```
Accepted values of `<test>`:
- histeq
- floodfill
- thresh
- morph
- all

## Implementation
The functions are accessed via functions/pixelfuncs.hpp, and split into several implementation files.

### Histogram Equalization
`void histeq(cv::Mat &img)`

First calculates the PDF of the image, then the CDF.
Uses the CDF's value to normalize the pixel intensities.


### Floodfill
`void floodfill(cv::Mat &img_in, cv::Mat &img_out, cv::Point2i &seed, uchar fill_color)`

Fills a region in `img_out` according to regions of `in`.
Uses the frontier fill formula, with the frontier implemented as a vector of `Point2i`.

`void floodfill(cv::Mat &img_in, cv::Point2i &seed, uchar fill_color)`

in-place version of above.


### Thresholding
`unsigned char ridler_calvard(cv::Mat &img)`

Performs thresholding with the Ridler-Calvard algorithm.
Begins by calculating the image's PDF and using it to calculate `m_0` and `m_1.`
(explain this more once you understand the code again)

`void simple_thresh(cv::Mat &img, unsigned char t)`

Performs simple thresholding with value `t`.
Pixels less than or equal to `t` will be suppressed, the rest will be set to max intensity.

`void double_thresh(cv::Mat &img, cv::Mat &d_thr)`

Performs double thresholding using `t_lo` and `t_hi`.
`t_lo` is calculated with the Ridler-Calvard algorithm, and `t_hi` is calculated from `t_lo`.
The algorithm performs floodfills with the low-threshold image `img_lo`, but outputs them into a new image `d_thr`.

### Morphological Operations
`void erosion(cv::Mat &img_in, cv::Mat &img_out, bool b8)`

`void dilation(cv::Mat &img_in, cv::Mat &img_out, bool b8)`

Performs erosion/dilation on `img_in` and sends the result to `img_out`.
Uses `std::vector<Point2i> se` as the structuring element, with `b8` determining whether to use B~4 or B~8.
For each pixel `p` of the image, checks every point in `se` relative to `p`.


## Bugs & Errors
None that I'm aware of