#pragma once

#include "Mat.hpp"

namespace smk
{
    Mat makeHighpassFilter();
    Mat makeSharpenFilter();
    Mat makeEmbossFilter();

    Mat makeBoxFilter(int w);
    Mat makeGaussianFilter(float sigma);
    Mat makeGaussianFilter1D(float sigma);
    void smoothImage(Mat const& src, Mat& dst, Mat& tmp, float sigma);
    void smoothImage(Mat const& src, Mat& dst, float sigma);
    Mat smoothImage(Mat const& src, float sigma);

    // sobel gradient
    Mat makeSobelFilter(bool horizontal);
    void gradientSingleChannel(Mat const& src, Mat& gx, Mat& gy);
    void gradient(Mat const& src, Mat& gx, Mat& gy);
    void gradientMagnitudeAngle(Mat const& src, Mat& mag, Mat& theta);

    // convolution
    void convolve(Mat const& src, Mat& dst, Mat const& filter, bool const preserve = true);
    Mat convolve(Mat const& src, Mat const& filter, bool preserve = true);

    // canny edge detection
    // tmin - min threshold
    // tmax - max threshold
    // sigma - gaussian smooth sigma
    void canny(Mat const& src, Mat& dst, const float tmin, const float tmax,  const float sigma);
}
