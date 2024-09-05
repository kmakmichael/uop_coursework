#include "region.hpp"


std::vector<region> image_analysis(cv::Mat &img) {
    std::set<uchar> colors;
    cv::MatConstIterator_<uchar> iter = img.begin<uchar>();
    for(; iter != img.end<uchar>(); iter++) {
        colors.insert(*iter);
    }

    // regions
    std::vector<region> regions;
    auto cit = colors.cbegin();
    cit++; // skip 0
    for(; cit != colors.cend(); cit++) {
        regions.push_back(analyze_region(img, *cit));
    }
    return regions;
}


region analyze_region(cv::Mat &img, uchar color) {
    region r;
    r.color = color;
    r.m = moments(img, color);
    cv::Point2i cen(r.m.value<double>(1,0) / r.m.value<double>(0,0), r.m.value<double>(0,1) / r.m.value<double>(0,0));
    r.centroid = cen;
    r.u = central_moments(img, color, cen);
    /*int sz [2] = {3, 3};
    cv::SparseMat cm(2, sz, CV_64F);
    cm.ref<double>(0, 0) = r.m.value<double>(0,0);
    cm.ref<double>(1, 1) = r.m.value<double>(1,1) - r.centroid.y * r.m.value<double>(1,0); // m_11 -y_c * m_10
    cm.ref<double>(2, 0) = r.m.value<double>(2,0) - r.centroid.x * r.m.value<double>(1,0); // m_20 -x_c * m_10
    cm.ref<double>(0, 2) = r.m.value<double>(0,2) - r.centroid.y * r.m.value<double>(0,1); // m_02 -y_c * m_01
    */
    // r.u = cm;
    r.eigen = eigen(r.u);
    r.dir = direction(r.u);
    r.ecc = eccentricity(r.u);
    return r;
}


cv::SparseMat moments(cv::Mat &region, uchar color) {
    int sz [2] = {3, 3};
    cv::SparseMat m(2, sz, CV_64F);
    m.ref<double>(0,0) = moment(region, color, 0, 0);
    m.ref<double>(0,1) = moment(region, color, 0, 1);
    m.ref<double>(1,0) = moment(region, color, 1, 0);
    m.ref<double>(1,1) = moment(region, color, 1, 1);
    m.ref<double>(2,0) = moment(region, color, 2, 0);
    m.ref<double>(0,2) = moment(region, color, 0, 2);
    return m;
}

cv::SparseMat central_moments(cv::Mat &region, uchar color) {
    int sz [2] = {3, 3};
    cv::SparseMat cm(2, sz, CV_64F);
    cm.ref<double>(0, 0) = central_moment(region, color, 0, 0);
    cm.ref<double>(1, 1) = central_moment(region, color, 1, 1);
    cm.ref<double>(2, 0) = central_moment(region, color, 2, 0);
    cm.ref<double>(0, 2) = central_moment(region, color, 0, 2);
    return cm;
}

cv::SparseMat central_moments(cv::Mat &region, uchar color, cv::Point2f cen) {
    int sz [2] = {3, 3};
    cv::SparseMat cm(2, sz, CV_64F);
    cm.ref<double>(0, 0) = central_moment(region, color, 0, 0, cen);
    cm.ref<double>(1, 1) = central_moment(region, color, 1, 1, cen);
    cm.ref<double>(2, 0) = central_moment(region, color, 2, 0, cen);
    cm.ref<double>(0, 2) = central_moment(region, color, 0, 2, cen);
    return cm;
}

void print_region_info(region r) {
    printf("color: %d\n", r.color);
    printf("centroid: (%d, %d)\n", r.centroid.x, r.centroid.y);
    printf("eigenvalues: (%0.2f, %0.2f)\n", r.eigen.first, r.eigen.second);
    printf("direction: %0.2f\n", r.dir);
    printf("eccentricity: %0.2f\n", r.ecc);
    printf("(major, minor) axes: (%0.2f, %0.2f)\n", 2*sqrt(r.eigen.first), 2*sqrt(r.eigen.second));
    // just to help with this ungodly printing process
    #define PM(mat,x,y) printf("m_%d%d = %.2f\n", x, y, mat.value<double>(x,y))
    // moments
    PM(r.m, 0, 0);
    PM(r.m, 0, 1);
    PM(r.m, 1, 0);
    PM(r.m, 1, 1);
    PM(r.m, 2, 0);
    PM(r.m, 0, 2);
    #undef PM
    #define PCM(mat,x,y) printf("u_%d%d = %.2f\n", x, y, mat.value<double>(x,y))
    // central moments
    PCM(r.u, 0, 0);
    PCM(r.u, 1, 1);
    PCM(r.u, 2, 0);
    PCM(r.u, 0, 2);
    double easy_u20 = r.m.value<double>(2,0) - r.centroid.x * r.m.value<double>(1,0); // m_20 -x_c*m_10
    double easy_u02 = r.m.value<double>(0,2) - r.centroid.y * r.m.value<double>(0,1); // m_02 -y_c*m_01
    printf("easy central moments (u20,u02): (%0.2f, %0.2f)\n", easy_u20, easy_u02);
    printf("major: %0.2fpx\n", sqrt(r.eigen.first));
    printf("minor: %0.2fpx\n", sqrt(r.eigen.second));
    #undef PCM
}