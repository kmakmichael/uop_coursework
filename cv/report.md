# ECPE 291: Computer Vision
Results from a guided independent study course regarding computer vision techniques.

- Author: Michael Kmak
- Advisor: Dr Pallipuram
- Github link: [https://github.com/kmakmichael/computer_vision](https://github.com/kmakmichael/computer_vision)



## OpenCV Basics
   The basis of OpenCV's library is the `Mat`, which defines a matrix of arbitrary dimensions.
The `Mat` object itself defines a header that points to a region of memory containing the data, allowing the user to edit the matrix, make sub-matrices, and more without having to fully re-allocate.
OpenCV handles all of the memory allocations and garbage collection, so there's no worrying about missing a `malloc()` or `free()`.
`Mat` also has static Matlab-style initializers like `Mat::zeros()` and `Mat::ones()`.
The type of a `Mat` can be one of many that OpenCV offers, typically defined as `CV_[bits][type]C[channels]`.
For example, these projects make frequent use of `CV_8UC1` - a 1-channel unsigned character matrix - for grayscale images.
Several predefined `Mat` types are also available, though are not used until Project 3.
The above example can be assigned to a `Mat1b` - a Mat that holds `uchar`.
These are more convenient than the templated `Mat_` for functions that only work in grayscale or color, not both.

There are several ways to access data in a `Mat`.
The `Mat::at()` function provides a reference to an element at a certain position, and the `Mat::ptr()` similarly provides a pointer to an element.
These functions can be called with a variety of arguments, and I found `at(int row, int col)` and `at(Point p)` to be the most useful.
The `Point` class defines a 2D coordinate.
Similar to `Mat`, there are some predefined Point types, and I found `Point2i` (integer coordinates) to be the most useful since we're working with pixels.
 
The `MatIterator` classes are also helpful for accessing data with these matrices.
They can be most easily accessed with the `Mat::begin()` and `Mat::end()` functions.
`MatIterators` are smart enough to iterate in order and will not get lost if the `Mat`'s data does not occupy a continuous chunk of memory.
They're compatible with STL functions like `sort()`, since they're random access iterators, which was useful for the implementation of hysteresis in Project 3.

## Project 1: Pixel Processing Functions

### Overview
This project implements the following pixel processing functions:
- histogram equalization
- floodfill
- thresholding
- dilation
- erosion

### Compile and Run:
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

### Implementation
The functions are accessed via functions/pixelfuncs.hpp, and split into several implementation files.

#### Histogram Equalization
`void histeq(cv::Mat &img)`

First calculates the PDF of the image, then the CDF.
Uses the CDF's value to normalize the pixel intensities.


#### Floodfill
`void floodfill(cv::Mat &img_in, cv::Mat &img_out, cv::Point2i &seed, uchar fill_color)`

Fills a region in `img_out` according to regions of `in`.
Uses the frontier fill formula, with the frontier implemented as a vector of `Point2i`.

`void floodfill(cv::Mat &img_in, cv::Point2i &seed, uchar fill_color)`

in-place version of above.


#### Thresholding
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

#### Morphological Operations
`void erosion(cv::Mat &img_in, cv::Mat &img_out, bool b8)`

`void dilation(cv::Mat &img_in, cv::Mat &img_out, bool b8)`

Performs erosion/dilation on `img_in` and sends the result to `img_out`.
Uses `std::vector<Point2i> se` as the structuring element, with `b8` determining whether to use B~4 or B~8.
For each pixel `p` of the image, checks every point in `se` relative to `p`.


### Bugs & Errors
None that I'm aware of

## Project 2: Object Classification

### Overview
Project 2 uses the functions from Project 1 in conjunction with new ones to classify apples, tangerines, and bananas in the provided fruit images.
The relevant functions are split between several `.hpp` files, titled according to their function.

### Compile and Run:
To compile, make sure you have `cmake` installed:
```
cmake --build .
make
```
To run:
```
./detection <image>
```

### Implementation
First, the foreground and background are separated via double thresholding followed by erosion and dilation.
The thresholding marks a rough difference between foreground and background, and the erosion and dilation help to clean the noise and smooth the edges.
The connected components algorithm is implemented with both floodfill and union find.
The image is then split into objects and each is analyzed for its moments, central moments, eigenvalues, direction, and eccentricity.
For each object, the axis lengths are calculated and drawn onto the image before the wall-following function generates a mask of the object.
The object is classified from the eccentricity and eigenvalues, and the mask is then used to color the original image around the object's borders.

#### Seperation
`void seperation(cv::Mat &img)`

Double thresholding followed by erosion and dilation help create clear seperation between foreground and background.
The process fails to get a clean seperation, unfortunately.

#### Connected Components
`size_t cc_floodfill(cv::Mat &input, cv::Mat &output)`

Uses floodfill to seperate foreground and background.
The algorithm iterates through the output image and applies a floodfill at every point where the input pixel is ON (255) and the output pixel is not.
Thus, a floodfill is applied to every foreground object.
A color for this object is used according to how many objects have been filled so far.
After 8 objects, the code will just use `255` for the color.
This was so that the objects would be visible when I was checking the function's output and was never adjusted.

`size_t cc_union(cv::Mat &input, cv::Mat &output)`

The union find algorithm iterates through the image twice, checking the pixels above and to the left of each pixel.
Preprocessor macros `C(x)` and `I(x)` are used to make the code look a bit more like the math.
The first loop creates groups and equivalences between the groups, and the second loop replaces these equivalences.

The equivalence matrix is implemented as a vector, where the value stored at each index represents a single equivalence.
As group number increases, equivalences are applied within the vector.
The thought process here is that, when multiple equivalences exist, they can all be traced back to a common group.
This common group will be the smallest equivalence.
When a new equivalence is found, the table is checked and the new equivalence is stored only if it's smaller than the current.
In essence, the equivalence table is traversed and simplified during the loop rather than after.
However I have no idea if this logic really checks out because this function will sometimes split an object in two.
It's likely that some chains of equivalences are broken by changing the table before the first iteration's completion.

`uint16_t eq_search(const std::vector<uint16_t> &table, size_t idx)`

Recursively searches an equivalence table for the smallest value.

#### Region Analysis
`enum Fruit`

Defines types of fruit for classification.
`mystery` serves as a fallback fruit if things go wrong.

`typedef struct ... region`

Defines a region and its properties.
Contains the results of PCA, the color of the region (assigned from connected components), and the moments.
Moments are stored in an OpenCV `SparseMat`, which represents a sparse matrix.
m~uv~ can be accessed as `region.m(u,v)`

`std::vector<region> image_analysis(cv::Mat &img)`

Determines how many regions the image contains and calls `analyze_region` for each.

`region analyze_region(cv::Mat &img, uchar color)`

Calls the functions that perform each step of analysis.
Returns a `region` struct.

`cv::SparseMat moments(cv::Mat &region, uchar color)`

Calls the `moment()` function for every moment required.

`cv::SparseMat central_moments(cv::Mat &region, uchar color)`

`cv::SparseMat central_moments(cv::Mat &region, uchar color, cv::Point2f cen)`

Calls the `central_moment()` function for each required central moment.
The first version calculates the region's centroid.

`void print_region_info(region r)`

Helper function to print out info.

#### Moments
`double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q)`

`double central_moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q, cv::Point2f cen)`

Finds the central moment u~pq~ of a region, defined by `color`.
First version of the function calculates the region's centroid.
Simply iterates through a region and calculates the moment, only using pixels whose intensities match `color`.

`moment(cv::Mat &region, uchar color, uint8_t p, uint8_t q)`

Either calls one of the helper functions below or calculates a 2D moment.
Iterates through and applies the 2D moment formula, only using pixels whose intensities match `color`.

`double moment_x(cv::Mat &region, uchar color, uint8_t p)`

`double moment_y(cv::Mat &region, uchar color, uint8_t q)`

`double moment_00(cv::Mat &region, uchar color)`

Helper functions to calculate a moment in 1 or 0 dimensions.
Probably a relatively pointless optimization.

#### Principal Component Analysis
`std::pair<double, double> eigen(cv::SparseMat &u)`

Calculates eigenvalues from the given moments.
`u` should come from `central_moments()` or be similarly populated.

`std::pair<double, double> eigen(cv::Mat &img, uchar color)`

Calculates eigenvalues for a region and returns them in a `std::pair`.

`std::pair<double, double> sm_ax_len(std::pair<double, double> evals)`

Calculates and returns the semimajor and semiminor axis lengths.

`double direction(cv::SparseMat &u)`

Calculates the direction of a region from the given central moments `u`.
`u` should come from `central_moments()` or be similarly populated.

`double eccentricity(cv::SparseMat &u)`

Calculates the eccentricity of a region from the given central moments `u`.
`u` should come from `central_moments()` or be similarly populated.

#### Wall-following
`enum Direction`

Cardinal directions to define where the wall-following alorithm is moving.

`#define TURN_RIGHT(x)`, `#define TURN_LEFT(x)`

Adjusts direction `x` to "turn" right or left.

`#define R_RIGHT(x)`, `#define R_LEFT(x)`

Returns the direction to the right or left of `x`

`cv::Point2i find_start(cv::Mat &img, uchar color)`

Iterates through the image until it finds a pixel of `color`, returns that position.

`cv::Point2i dir2pt(Direction d)`

`cv::Point2i dir2pt(int i)`

Returns a Point that can serve as an offset in the given direction.

`cv::Mat wall(cv::Mat &img, uchar color)`

Implements the wall-following algorithm.
Returns a mask of on and off pixels corresponding to the original image.
This mask can be used as an argument in `cv::Mat::setTo()` to set all pixels in the main image that correspond with on pixels in the mask to a given value.
First, initializes the mask and finds the start pixel.
The loop operates on the assumption that the algorithm will eventually find its way back to the start pixel and turn it on, at which point the loop will end.

#### Classification
If the eccentricity is below 0.8, then the object is likely to be a banana.
If the object is not a banana, then the eigenvalues are checked to determine whether it is an apple or tangerine.

### Bugs & Errors
- Seperation is not clean
- Floodfill connected components only reliably handles 8 images
- Union find connected components "finds" extra objects and will count holes as objects

## Project 3: Canny Edge Detection

### Overview
Project 3 utilizes the canny edge detection algorithm to detect and illustrate the edges of an image.
The image is convolved with 1D gaussian kernels and derivatives and the resulting matrices are used to find the magnitude and direction at each point.
Magnitude and direction are used in suppression, and the result is sent through hysteresis and edge linking.
This project is incomplete and suffers from bugs/errors.

### Compile and Run:
To compile, make sure you have `cmake` installed:
```
cmake --build .
make
```
To run:
```
./canny <image> <sigma> <template to match>
```

### Implementation
The edge detection algorithm starts by generating kernels.
The kernels and derivatives are implemented as 1xN `Mat`, and are used in the convolution process.
The convolution process goes wrong when colvolving the gaussian derivatives, and this error proliferates through the rest of the process.
Using the convolved image, direction and magnitude are calculated for the suppression process.
For hysteresis, a clone of the post-suppression image is sorted using `std::sort()`.
After hysteresis, the image is sent through edge linking and converted back to `uchar`.

#### Kernels
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

#### Convolution
`cv::Mat1f convolve(cv::Mat_<T> img, cv::Mat1f kern)`

Iterates through `kern` on each each pixel of `img` to perform convolution.
`i` represents the current pixel in the output image `conv` being convolved, and `p` is the pixel where calculations are being done.
`p` is calculated by taking `i`'s position, adding `kshift` to offset the kernel's indexing, and adding the current position in the kernel's iteration loop.
The sum of values at `p` is written to `i`.
I don't believe the `abs` is necessary, but it was an attempt to alleviate negative values that come from a deeper undiagnosed problem.

#### Magnitude and Direction
Performed as part of the main loop.
One loop goes through and performs the calculations on each pixel for direction and magnitude.
Direction could be calculated without a loop by using OpenCV's matrix math functions, but there doesn't seem to be a function to replace `atan2()` so I just did both magnitude and direction the same way.
Both could also be accomplished with OpenCV's built-in `cartToPolar()` function.

#### Suppression
`cv::Mat1f suppress(cv::Mat1f direction, cv::Mat1f magnitude)`

Performs suppression by iterating through each pixel.
Suppresses pixels according to the magnitudes of two opposite surrounding pixels, which are chosen based on the direction.
Each pixel of the output matrix is first set to the original magnitude.
If it must be suppressed, it is then set to 0.

#### Hysteresis
`void hysteresis(cv::Mat1f img, float t_high, float t_low)`

Loops through a matrix and sets pixels according to the given thresholds.
Pixels more intense than `t_high` are set to 255, pixels less intense than `t_low` are set to 0, and the remainder are set to 125.

#### Edge Linking
`cv::Mat1f edge_linking(cv::Mat1f hyst)`

Performs edge linking.
Iterates through both the input matrix `hyst` and the output matrix `edge` in tandem.
If a pixel's intensity is 125, the surrounding pixels will be checked.
If any surrounding pixels are 255, then this pixel will be as well.
This is performed by iterating on the empty 3x3 matrix `omat` and using the position within that matrix as an offset.
Similarly to convolution, `opt` is used to counter the indexing of the 3x3 matrix when finding the surrounding pixels.
For example, to check the pixel above `h = (50, 60)`, add the position of the iterator `oit = (0,1)` to the offfset `opt = (-1,-1)` to get `p = (50, 59)`.
Pixels not equal to 125 are copied from `hyst` to `edge`.

### Bugs & Errors
- Gaussian derivatives aren't generated or applied correctly, causing most pixel values to be very close to 0 after convolution
- Errors with convolution are pushed through the entire process. There are likely more bugs but it's hard to tell when this one messes things up for the rest of the execution.

## Project 4: Mosaicking

### Overview
Attempts to perform image mosaicking with Matlab.
First, correspondences between the images must be found.
These were loaded from a file provided by Clemson's course website[^1].
Using these correspondences, the homography between two images can be calculated with the Direct Linear Transform algorithm.
The homography `H` is a 3x3 matrix such that `x' = Hx`.
That is, multiplying an image by `H` will warp it to fit the perspective of the other (fixed) image.
Then, correlations between the warped images must be found.
A correlation is how far offset an image is from another.
There are several methods to find this, and I attempted to implement the Sum of Squared Differences (SSD) function and Fourier-based alignment.
Neither implementations were successful, likely because I didn't quite understand them entirely.
After finding the correspondences, images must be stitched together and their edges feathered.
The featuring process smooths the transition between two images.
I was not able to get to this part of the code.


### Run:
To run (in a Matlab-capable environment):
```
mosaicking
```

### Implementation

#### Correspondences
To start, I reformatted the downloaded correspondences into a more Matlab-compatible form.
These can be found in `correspondences.m`, and are matrices of points that correspond to the same object or location between images.
For example, one set of points might correspond to the center of the clock.
These points are read into a 12x12 cell.

#### Homography
To calculate homography, each pair of points is transformed into a 2x9 matrix.
These 2x9 matrices are then assembled into an nx9 matrix A.
AH=0 can be solved to find the 9x1 matrix `H`, which can be rearranged to a 3x3.
However, you can also use the SVD of A, A=UDV^T^.
The homography matrix h is the last column of V.
This is retrieved and reshaped to a 3x3 matrix.

#### Correlation
##### SSD
SSD Correlation seeks to minimize the SSD function Essd(u), where u is the offset between the two images.
This was hardly implemented because I ran out of time.

##### Fourier-based Alignment
Another method of finding the correlation is to maximize the product of the two aligned images.
The correlation can be found with a complex equation explained in Szeleski's paper[^2], but it can also be found with Fourier transforms.
The Fourier transform of the correlation is equal to the Fourier transform of the fixed image times the Fourier transform of the complex conjugate of the moving image.
In Matlab terms, `fft(moving) .* fft(conj(fixed))`.
I assumed that maximizing this function would result in the proper correlation, but the values I got were not correct.

### Bugs & Errors
- homography calculations seem to be inaccurate. warped images don't line up well when stitched together manually
- correlation does not work

### References
[^1] [Clemson's ECE 877 Course, HW #4](https://cecas.clemson.edu/~stb/ece877/spring2012/)

[^2] [Image Alignment and Stitching: A Tutorial - Rick Szeliski](https://www.microsoft.com/en-us/research/publication/image-alignment-and-stitching-a-tutorial/?from=https%3A%2F%2Fresearch.microsoft.com%2Fpubs%2F70092%2Ftr-2004-92.pdf)

[^3] [StackExchange](https://math.stackexchange.com/questions/494238/how-to-compute-homography-matrix-h-from-corresponding-points-2d-2d-planar-homog)