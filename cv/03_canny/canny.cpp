#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <cmath>
/*
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <cinttypes>
#include <cstdio>
*/
#include "kernels.hpp"
#include "convolution.hpp"
#include "proc.hpp"

#define WRITE_IMGS

void pshow(cv::Mat1f img, const char *title, const char *filename);
void show_img(cv::Mat img, const char *title, const char *filename);

int main(int argc, char *argv[]) {

    // handle params
	if (argc > 4 || argc == 1) {
		fprintf(stderr, "usage: canny <image> <sigma> <template match (optional)>\n");
		return 1;
	}

    char *file_in = argv[1];
    float sigma;

    if (argc > 2) {
        sigma = atof(argv[2]);
        printf("sigma = %f\n", sigma);
    } else {
        sigma = 1.0;
        printf("no sigma given, assuming %f\n", sigma);
    }

    // read the given image
    cv::Mat1b original = cv::imread(file_in, cv::IMREAD_GRAYSCALE);
    if  (original.empty()) {
        return 1;
    }

    if (argc == 4) {
        // probably call a seperate function for this vs non-matching
        printf("template image given: %s\n", argv[4]);
        
    } else {
        printf("performing edge matching\n");
        // kernels
        cv::Mat1f h_kern = gaussian(sigma);
        cv::Mat1f v_kern;
        cv::transpose(h_kern, v_kern);
        cv::Mat1f h_deriv = deriv(sigma);
        cv::Mat1f v_deriv;
        cv::transpose(h_deriv, v_deriv);
        print_kern(h_kern);
        print_kern(v_kern);
        print_kern(h_deriv);
        print_kern(v_deriv);

        // convolution
        cv::Mat1b tw;
        cv::Mat1f temp = convolve<uchar>(original, h_kern);
        cv::Mat1f hori = convolve<float>(temp, h_deriv);
        hori.convertTo(tw, CV_8UC1);
        show_img(tw, "Temp Hori", "temp_h.bmp");
        
        temp = convolve<uchar>(original, v_kern);
        cv::Mat1f vert = convolve<float>(temp, v_deriv);
        vert.convertTo(tw, CV_8UC1);
        show_img(tw, "Temp Vert", "temp_v.bmp");

        
        // direction and magnitude
        cv::Mat1f mag = cv::Mat::zeros(original.rows, original.cols, CV_32FC1);
        cv::Mat1f dir = cv::Mat::zeros(original.rows, original.cols, CV_32FC1);
        for(int i = 0; i < original.total(); i++) {
            mag.at<float>(i) = sqrt((hori.at<float>(i) * hori.at<float>(i)) + (vert.at<float>(i) * vert.at<float>(i)));
            dir.at<float>(i) = atan2(hori.at<float>(i), vert.at<float>(i));
            // printf("atan of %0.2f/%0.2f = %0.2f\n", hori.at<float>(i), vert.at<float>(i), dir.at<float>(i));
        }
        show_img(mag, "Magnitude", "magnitude.bmp");
        show_img(dir, "Direction", "direction.bmp");

        // suppression
        cv::Mat1f supp = suppress(mag, dir);
        show_img(supp, "Non-Maximal Suppression Image", "suppression.bmp");

        // hysteresis
        temp = supp.clone();
        std::sort(temp.begin(), temp.end());
        show_img(temp, "Sorted Temp", "sorted.bmp");
        float t_high = temp.at<float>((int)(temp.rows * temp.cols * 0.9));
        float t_low = t_high * 0.2;
        hysteresis(supp, t_high, t_low);
        show_img(supp, "Hysteresis", "hysteresis.bmp");

        // edge linking
        cv::Mat1f edges = edge_linking(supp);
        edges.convertTo(tw, CV_8UC1);
        show_img(edges, "Edges", "edges.bmp");
    }

    // cv::Mat img_color = cv::imread(file_in, cv::IMREAD_COLOR);
    show_img(original, "Original", "orig.bmp");
}


void pshow(cv::Mat1f img, const char *title, const char *filename) {
    cv::Mat1b tw(img.rows, img.cols, CV_8UC1);
    for (auto i = img.begin(); i != img.end(); i++) {
        if (*i > 1) {
            tw.at<uchar>(i.pos()) = static_cast<uchar>(*i);
        } else {
            tw.at<uchar>(i.pos()) = *i * 255;
        }
    }
    #ifdef WRITE_IMGS
        printf("writing to %s\n", filename);
        cv::imwrite(filename, tw);
    #endif
    cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
    cv::imshow(title, tw);
}

void show_img(cv::Mat img, const char *title, const char *filename) {
    #ifdef WRITE_IMGS
        printf("writing to %s\n", filename);
        cv::imwrite(filename, img);
    #endif
    cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
    cv::imshow(title, img);
}
