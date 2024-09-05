# Project 3: Canny Edge Detection

## Overview
Project 3 utilizes the canny edge detection algorithm to detect and illustrate the edges of an image.
The image is convolved with 1D gaussian kernels and derivatives and the resulting matrices are used to find the magnitude and direction at each point.
Magnitude and direction are used in suppression, and the result is sent through hysteresis and edge linking.
This project is incomplete and suffers from bugs/errors.

## Compile and Run:
To compile, make sure you have `cmake` installed:
```
cmake --build .
make
```
To run:
```
./canny <image> <sigma> <template to match>
```

## Implementation
The edge detection algorithm starts by generating kernels.
The kernels and derivatives are implemented as 1xN `Mat`, and are used in the convolution process.
The convolution process goes wrong when colvolving the gaussian derivatives, and this error proliferates through the rest of the process.
Using the convolved image, direction and magnitude are calculated for the suppression process.
For hysteresis, a clone of the post-suppression image is sorted using `std::sort()`.
After hysteresis, the image is sent through edge linking and converted back to `uchar`.

### Kernels
`cv::Mat1f gaussian(float sigma)`

Generates a horizontal kernel.
To get the vertical kernel, use `cv::transpose()` on a horizontal kernel.
One loop sets values and calculates a sum, the second divides by that sum.

`cv::Mat1f deriv(float sigma)`

Generates a horizontal kernel derivative.
To get the vertical derivative, use `cv::transpose()` on a horizontal derivative.
One loop sets values and calculates a sum, the second divides by that sum, and the third loop flips the kernel.

`void print_kern(cv::Mat1f kern)`

Prints `kern`.

### Convolution
`cv::Mat1f convolve(cv::Mat_<T> img, cv::Mat1f kern)`

Iterates through `kern` on each each pixel of `img` to perform convolution.
`i` represents the current pixel in the output image `conv` being convolved, and `p` is the pixel where calculations are being done.
`p` is calculated by taking `i`'s position, adding `kshift` to offset the kernel's indexing, and adding the current position in the kernel's iteration loop.
The sum of values at `p` is written to `i`.
I don't believe the `abs` is necessary, but it was an attempt to alleviate negative values that come from a deeper undiagnosed problem.

### Magnitude and Direction
Performed as part of the main loop.
One loop goes through and performs the calculations on each pixel for direction and magnitude.
Direction could be calculated without a loop by using OpenCV's matrix math functions, but there doesn't seem to be a function to replace `atan2()` so I just did both magnitude and direction the same way.
Both could also be accomplished with OpenCV's built-in `cartToPolar()` function.

### Suppression
`cv::Mat1f suppress(cv::Mat1f direction, cv::Mat1f magnitude)`

Performs suppression by iterating through each pixel.
Suppresses pixels according to the magnitudes of two opposite surrounding pixels, which are chosen based on the direction.
Each pixel of the output matrix is first set to the original magnitude.
If it must be suppressed, it is then set to 0.

### Hysteresis
`void hysteresis(cv::Mat1f img, float t_high, float t_low)`

Loops through a matrix and sets pixels according to the given thresholds.
Pixels more intense than `t_high` are set to 255, pixels less intense than `t_low` are set to 0, and the remainder are set to 125.

### Edge Linking
`cv::Mat1f edge_linking(cv::Mat1f hyst)`

Performs edge linking.
Iterates through both the input matrix `hyst` and the output matrix `edge` in tandem.
If a pixel's intensity is 125, the surrounding pixels will be checked.
If any surrounding pixels are 255, then this pixel will be as well.
This is performed by iterating on the empty 3x3 matrix `omat` and using the position within that matrix as an offset.
Similarly to convolution, `opt` is used to counter the indexing of the 3x3 matrix when finding the surrounding pixels.
For example, to check the pixel above `h = (50, 60)`, add the position of the iterator `oit = (0,1)` to the offfset `opt = (-1,-1)` to get `p = (50, 59)`.
Pixels not equal to 125 are copied from `hyst` to `edge`.

## Bugs & Errors
- Gaussian derivatives aren't generated or applied correctly, causing most pixel values to be very close to 0 after convolution
- Errors with convolution are pushed through the entire process. There are likely more bugs but it's hard to tell when this one messes things up for the rest of the execution.



