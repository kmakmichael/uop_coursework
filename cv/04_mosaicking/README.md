# Project 4: Mosaicking

## Overview
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


## Run:
To run (in a Matlab-capable environment):
```
mosaicking
```

## Implementation

### Correspondences
To start, I reformatted the downloaded correspondences into a more Matlab-compatible form.
These can be found in `correspondences.m`, and are matrices of points that correspond to the same object or location between images.
For example, one set of points might correspond to the center of the clock.
These points are read into a 12x12 cell.

### Homography
To calculate homography, each pair of points is transformed into a 2x9 matrix.
These 2x9 matrices are then assembled into an nx9 matrix A.
AH=0 can be solved to find the 9x1 matrix `H`, which can be rearranged to a 3x3.
However, you can also use the SVD of A, A=UDV^T^.
The homography matrix h is the last column of V.
This is retrieved and reshaped to a 3x3 matrix.

### Correlation
#### SSD
SSD Correlation seeks to minimize the SSD function Essd(u), where u is the offset between the two images.
This was hardly implemented because I ran out of time.

#### Fourier-based Alignment
Another method of finding the correlation is to maximize the product of the two aligned images.
The correlation can be found with a complex equation explained in Szeleski's paper[^2], but it can also be found with Fourier transforms.
The Fourier transform of the correlation is equal to the Fourier transform of the fixed image times the Fourier transform of the complex conjugate of the moving image.
In Matlab terms, `fft(moving) .* fft(conj(fixed))`.
I assumed that maximizing this function would result in the proper correlation, but the values I got were not correct.

## Bugs & Errors
- homography calculations seem to be inaccurate. warped images don't line up well when stitched together manually
- correlation does not work

## References
[^1] [Clemson's ECE 877 Course, HW #4](https://cecas.clemson.edu/~stb/ece877/spring2012/)

[^2] [Image Alignment and Stitching: A Tutorial - Rick Szeliski](https://www.microsoft.com/en-us/research/publication/image-alignment-and-stitching-a-tutorial/?from=https%3A%2F%2Fresearch.microsoft.com%2Fpubs%2F70092%2Ftr-2004-92.pdf)

[^3] [StackExchange](https://math.stackexchange.com/questions/494238/how-to-compute-homography-matrix-h-from-corresponding-points-2d-2d-planar-homog)