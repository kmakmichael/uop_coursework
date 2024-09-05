#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/interface.h>
#include <cstdio>
#include <cstring>

#include "functions/pixelfuncs.hpp"

/*
    Test program for 7 different pixel processing functions
        usage: unit_1 <input> <section>
            input: .bmp file
            section: "histeq", "floodfill", "thresh", "morph", "all"
*/

void section_1(cv::Mat &img) {
    histeq(img);
    cv::namedWindow("histeq", cv::WINDOW_AUTOSIZE);
    cv::imshow("histeq", img);
    #ifndef WRITE_IMG
        printf("writing to histeq.bmp\n");
        cv::imwrite("histeq.bmp", img);
    #endif
}


void section_2(cv::Mat &img) {
    size_t px_x = 0, px_y = 0;
    printf("X: ");
    scanf("%zu", &px_x);
    std::fflush(stdin);
    printf("Y: ");
    scanf("%zu", &px_y);
    while (px_x >= img.rows || px_y >= img.cols) {
        printf("coords out of bounds, try again\n");
        printf("X: ");
        scanf("%zu", &px_x);
        std::fflush(stdin);
        printf("Y: ");
        scanf("%zu", &px_y);
    }
    cv::Point2i pt(px_x, px_y);
    floodfill(img, pt, 255);
    cv::namedWindow("floodfill", cv::WINDOW_AUTOSIZE);
    cv::imshow("floodfill", img);
    #ifndef WRITE_IMG
        printf("writing to floodfill.bmp\n");
        cv::imwrite("floodfill.bmp", img);
    #endif
}

void section_3(cv::Mat &img) {
    cv::Mat threshed = cv::Mat::zeros(img.rows, img.cols, img.type());
    double_thresh(img, threshed);
    cv::namedWindow("double_thresh", cv::WINDOW_AUTOSIZE);
    cv::imshow("double_thresh", threshed);
    #ifndef WRITE_IMG
        printf("writing to double_thresh.bmp\n");
        cv::imwrite("double_thresh.bmp", threshed);
    #endif
}

void section_4(cv::Mat &img) {
    cv::Mat thresh = cv::Mat::zeros(img.rows, img.cols, img.type());
    cv::Mat erode = cv::Mat::zeros(img.rows, img.cols, img.type());
    cv::Mat dilate = cv::Mat::zeros(img.rows, img.cols, img.type());
    double_thresh(img, thresh);
    erosion(thresh, erode, false);
    dilation(thresh, dilate, false);
    cv::namedWindow("erosion", cv::WINDOW_AUTOSIZE);
    cv::imshow("erosion", erode);
    cv::namedWindow("dilation", cv::WINDOW_AUTOSIZE);
    cv::imshow("dilation", dilate);
    #ifndef WRITE_IMG
        printf("writing to erode.bmp, dilate.bmp\n");
        cv::imwrite("erode.bmp", erode);
        cv::imwrite("dilate.bmp", dilate);
    #endif
}

int main(int argc, char *argv[]) {
    bool result;

    // handle params
	if (argc != 3) {
		fprintf(stderr, "usage: test <input> <section>\n");
		return 1;
	}

    char *func = argv[2];
    char *file_in = argv[1];

    // read the given image
    cv::Mat img = cv::imread(file_in, cv::IMREAD_GRAYSCALE);
    if  (img.empty()) {
        return 1;
    }
    
    // call the proper function
    fprintf(stdout, "running %s test using %s\n", func, file_in);
    if (!strcmp(func, "histeq")) {
        section_1(img);
    } else if (!strcmp(func, "floodfill")) {
        section_2(img);
    } else if (!strcmp(func, "thresh")) {
        section_3(img);
    } else if (!strcmp(func, "morph")) {
        section_4(img);
    } else if (!strcmp(func, "all")) {
        section_1(img);
        // reset img because histeq modifies it
        img = cv::imread(file_in, cv::IMREAD_GRAYSCALE);
        section_2(img);
        section_3(img);
        section_4(img);
    } else {
        fprintf(stderr, "%s is not a valid section\n", func);
        return 1;
    }
    return 0;
}