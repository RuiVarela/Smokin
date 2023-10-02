#pragma once

#include "Mat.hpp"

namespace smk
{
  // Make an integral image or summed area table from an image
  // image im: image to process
  // returns: image I such that I[x,y] = sum{i<=x, j<=y}(im[i,j])
  void makeIntegralImage(Mat const& im, Mat& out);

  //
  // Gets an integral image info for a region
  // c - channel
  // ltrb - Corners
  // sum = sumed area
  // count = pixed count used in sum
  //
  void getIntegralImageRegion(Mat const& im, int c,
                              int l, int t, int r, int b,
                              float& sum, int& count);

  // Apply a box filter to an image using an integral image for speed
  // image im: image to smooth
  // int s: window size for box filter
  // returns: smoothed image
  void boxfilterIntegralImage(Mat const& im, int smooth, Mat& out);

  struct LucasKanade
  {
      // Calculate the time-structure matrix of an image pair.
      // image im: the input image.
      // image prev: the previous image in sequence.
      // int s: window size for smoothing.
      // returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
      //          3rd channel is IxIy, 4th channel is IxIt, 5th channel is IyIt.
      void timeStructureMatrix(Mat const &im, Mat const &prev, int smooth, Mat &S);

      // Calculate the velocity given a structure image
      // image S: time-structure image
      // int stride: only calculate subset of pixels for speed
      // v - output velocity image
      void velocityImage(Mat const &S, int stride, Mat &v);

      // Lucas–Kanade optical flow
      // Calculate the optical flow between two images
      // image im: current image
      // image prev: previous image
      // int smooth: amount to smooth structure matrix by
      // int stride: downsampling for velocity matrix
      // returns: velocity matrix
      void opticalflow(Mat const &im, Mat const &prev, int smooth, int stride, Mat &vs);

    private:
      Mat m_curr_gray;
      Mat m_prev_gray;

      Mat m_I, m_Ii, m_S;
      Mat m_V;
  };

} // namespace vs
