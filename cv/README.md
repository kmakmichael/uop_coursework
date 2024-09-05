# ECPE 291: Computer Vision
Results from a guided independent study course regarding computer vision techniques.

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
This project implements the following pixel processing functions:
- histogram equalization
- floodfill
- thresholding
- dilation
- erosion

## Project 2: Object Classification
First, the foreground and background are separated via double thresholding followed by erosion and dilation.
The thresholding marks a rough difference between foreground and background, and the erosion and dilation help to clean the noise and smooth the edges.
The connected components algorithm is implemented with both floodfill and union find.
The image is then split into objects and each is analyzed for its moments, central moments, eigenvalues, direction, and eccentricity.
For each object, the axis lengths are calculated and drawn onto the image before the wall-following function generates a mask of the object.
The object is classified from the eccentricity and eigenvalues, and the mask is then used to color the original image around the object's borders.


## Project 3: Canny Edge Detection
Project 3 utilizes the canny edge detection algorithm to detect and illustrate the edges of an image.
The image is convolved with 1D gaussian kernels and derivatives and the resulting matrices are used to find the magnitude and direction at each point.
Magnitude and direction are used in suppression, and the result is sent through hysteresis and edge linking.
This project is incomplete and suffers from bugs/errors.


## Project 4: Mosaicking
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
