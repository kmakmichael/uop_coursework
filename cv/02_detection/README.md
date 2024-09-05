# Project 2: Object Classification

## Overview
Project 2 uses the functions from Project 1 in conjunction with new ones to classify apples, tangerines, and bananas in the provided fruit images.
The relevant functions are split between several `.hpp` files, titled according to their function.

## Compile and Run:
To compile, make sure you have `cmake` installed:
```
cmake --build .
make
```
To run:
```
./detection <image>
```

## Implementation
First, the foreground and background are separated via double thresholding followed by erosion and dilation.
The thresholding marks a rough difference between foreground and background, and the erosion and dilation help to clean the noise and smooth the edges.
The connected components algorithm is implemented with both floodfill and union find.
The image is then split into objects and each is analyzed for its moments, central moments, eigenvalues, direction, and eccentricity.
For each object, the axis lengths are calculated and drawn onto the image before the wall-following function generates a mask of the object.
The object is classified from the eccentricity and eigenvalues, and the mask is then used to color the original image around the object's borders.

### Seperation
`void seperation(cv::Mat &img)`

Double thresholding followed by erosion and dilation help create clear seperation between foreground and background.
The process fails to get a clean seperation, unfortunately.

### Connected Components
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

### Region Analysis
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

### Moments
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

### Principal Component Analysis
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

### Wall-following
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

### Classification
If the eccentricity is below 0.8, then the object is likely to be a banana.
If the object is not a banana, then the eigenvalues are checked to determine whether it is an apple or tangerine.

## Bugs & Errors
- Seperation is not clean
- Floodfill connected components only reliably handles 8 images
- Union find connected components "finds" extra objects and will count holes as objects