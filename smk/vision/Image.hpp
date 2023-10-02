#pragma once

#include "Mat.hpp"
#include "../Text.hpp"
#include "../math/Vector.hpp"
#include <array>

namespace smk
{
    Mat loadImage(str32 path, int channels = 0);
    bool saveImage(str32 path, Mat const &im);

    Mat loadImage(str8 path, int channels = 0);
    bool saveImage(str8 path, Mat const &im);


    void rgb2gray(Mat const& src, Mat &dst);
    Mat rgb2gray(Mat const& src);

    void rgb2bgr(Mat const& src, Mat &dst);
    Mat rgb2bgr(Mat const& src);
    void rgb2bgrInplace(Mat &inplace);

    void rgb2hsv(Mat const& src, Mat &dst);
    Mat rgb2hsv(Mat const& src);
    void rgb2hsvInplace(Mat &inplace);

    void hsv2rgb(Mat const& src, Mat &dst);
    Mat hsv2rgb(Mat const& src);
    void hsv2rgbInplace(Mat &inplace);


    enum ThresholdMode {
        Binary,
        BinaryInverted,
        Truncate,
        ToZero,
        ToZeroInverted
    };
    Mat::Type threshold(Mat const& src, Mat &dst, ThresholdMode const mode, Mat::Type const value, Mat::Type const max = 1.0f);
    Mat::Type thresholdOtsu(Mat const& src, Mat &dst, ThresholdMode const mode, Mat::Type const max = 1.0f);


    enum ResizeMode
    {
        NearestNeighbor,
        Bilinear
    };
    float interpolateNN(Mat const& im, float x, float y, int c);
    float interpolateBL(Mat const& im, float x, float y, int c);
    void resize(Mat const& src, Mat &dst, int nw, int nh, ResizeMode const mode = Bilinear);
    Mat resize(Mat const& src, int nw, int nh, ResizeMode const mode = Bilinear);


    Mat cylindricalProject(Mat const &im, float f);

    // http://dlib.net/imaging.html#extract_image_4points
    // The 4 points in pts define a convex quadrilateral and this function extracts
    // that part of the input image img and stores it into dst.  Therefore, each
    // corner of the quadrilateral is associated to a corner of dst and bilinear
    // interpolation and a projective mapping is used to transform the pixels in the
    // quadrilateral into dst.  To determine which corners of the quadrilateral map
    // to which corners of dst we fit the tightest possible rectangle to the
    // quadrilateral and map its vertices to their nearest rectangle corners.  These
    // corners are then trivially mapped to dst (i.e.  upper left corner to upper
    // left corner, upper right corner to upper right corner, etc.).
    void extractImage4points(Mat const& im, Mat &dst, const std::array<Vector2<float>,4>& points);
}

