#include <inttypes.h>
#include <utility>

#include "connected_components.hpp"
#include "pxfuncs/pixelfuncs.hpp"

uint16_t eq_search(const std::vector<uint16_t> &table, size_t idx);

size_t cc_floodfill(cv::Mat &input, cv::Mat &output) {
    // setup
    size_t num_components = 0;
    output = cv::Mat::zeros(input.rows, input.cols, input.type());

    // iteration
    cv::MatConstIterator_<uchar> iter_end = output.end<uchar>();
    cv::MatConstIterator_<uchar> iter = output.begin<uchar>();
    cv::Point2i n;
    for(; iter != iter_end; iter++) {
        if(*iter == 0 && input.at<uchar>(iter.pos()) == 255) {
            n = iter.pos();
            num_components++;
            if (num_components > 8) {
                // horrible way of handling things but deal with that later
                fprintf(stderr, "more than 8 components\n");
                floodfill(input, output, n, 255);
            }
            // printf("filling %zuth component with color %u\n", num_components, (uchar)(num_components*32-1));
            floodfill(input, output, n, (uchar)(num_components*32-1));
        }
    }
    return num_components;
}

size_t cc_union(cv::Mat &input, cv::Mat &output) {
    size_t num_components = 0;
    cv::Point2i up(0, -1), left(-1, 0);
    std::vector<uint16_t> equiv;
    output = cv::Mat::zeros(input.rows, input.cols, input.type());
    cv::Mat temp = cv::Mat::zeros(input.rows, input.cols, CV_16UC1);
    #define C(x) temp.at<uint16_t>(x)
    #define I(x) input.at<uchar>(x)

    /* 
        first loop 
    */
    cv::MatConstIterator_<uchar> iter = input.begin<uchar>();
    cv::Point2i p, u, l;
    equiv.push_back(num_components);
    iter++;
    for(; iter != input.end<uchar>(); iter++) {
        p = iter.pos();
        l = p + left;
        u = p + up;
        if (p.y > 0 && I(p) == I(u)) {
            C(p) = C(u);
            if (p.x > 0 && I(p) == I(l)) {
                if (equiv[C(l)] != C(l)) {
                    //printf("something's wrong\n");
                }
                if (eq_search(equiv, C(u)) < equiv[C(l)]) {
                    equiv[C(l)] = eq_search(equiv, C(u));
                }
            }
        } else if (p.x > 0 && I(p) == I(l)) {
            C(p) = C(l);
        } else {
            num_components++;
            C(p) = num_components;
            equiv.push_back(num_components);
        }
    }
    for(int i = 0; i < equiv.size(); i++) {
        //printf("equiv[%d] -> %d\n", i, equiv[i]);
    }
    //printf("found %zu components\n", num_components);

    /*
        second pass
    */
    cv::MatIterator_<uint16_t> i2 = temp.begin<uint16_t>();
    cv::Point2i n;
    for(; i2 != temp.end<uint16_t>(); i2++) {
        n = i2.pos();
        output.at<uchar>(n) = (255 * eq_search(equiv, *i2) / (float)equiv.size());
    }
    #undef C
    #undef I
    return num_components;
}


/*
    Recursively dig through an equivalence table, back to the very first equivalence
*/
uint16_t eq_search(const std::vector<uint16_t> &table, size_t idx) {
    if (table[idx] != idx) {
        return eq_search(table, table[idx]);
    } else {
        return idx;
    }

}