#pragma once

#include "Mat.hpp"
#include "../math/Vector.hpp"

#include <vector>


namespace smk
{
    // Calculate L2 distance between two points.
    // Minkowski distance between two points of order 2
    // https://en.wikipedia.org/wiki/Minkowski_distance
    // point p, q: points.
    // returns: L2 distance between them.
    inline static float distance(Vector2<float> const& p, Vector2<float> const& q) {
        Vector2<float> delta = q - p;
        return delta.length();
    }

    // A descriptor for a point in an image.
    // point p: x,y coordinates of the image pixel.
    // int n: the number of floating point values in the descriptor.
    // float *data: the descriptor for the pixel.
    struct Descriptor
    {
        Vector2<float> p;
        int n = 0;
        float *data = nullptr;

        void reshape(int size);

        // Create a feature descriptor for an index in an image.
        // very simple descriptor : its just a patch of neighbors pixels
        // image im: source image.
        // int i: index in image for the pixel we want to describe.
        // returns: descriptor for that index.
        static Descriptor describe(Mat const&im, int i);
        static float distance(Descriptor const& a, Descriptor const& b);

      private:
        std::shared_ptr<float> shared_data;
    };
    using Descriptors = std::vector<Descriptor>;

    // A match between two points in an image.
    // point p, q: x,y coordinates of the two matching pixels.
    // int ai, bi: indexes in the descriptor array. For eliminating duplicates.
    // float distance: the distance between the descriptors for the points.
    struct Match {
        Vector2<float> p;
        Vector2<float> q;
        int ai = -1;
        int bi = -1;
        float distance;
    };
    using Matches = std::vector<Match>;

    // Finds best matches between descriptors of two images.
    // descriptor *a, *b: array of descriptors for pixels in two images.
    // returns: best matches found. each descriptor in a should match with at most
    //          one other descriptor in b.
    Matches matchDescriptors(Descriptors const& a, Descriptors const& b);

    // Count number of inliers in a set of matches. Should also bring inliers to the front of the array.
    // matrix H: homography between coordinate systems.
    // match *m: matches to compute inlier/outlier.
    // float thresh: threshold to be an inlier.
    // returns: number of inliers whose projected point falls within thresh of
    //          their match in the other image. Should also rearrange matches
    //          so that the inliers are first in the array. For drawing.
    int modelInliers(Matd const& H, Matches& m, float thresh);

    // Randomly shuffle matches for RANSAC.
    // Fisher-Yate
    // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
    // match *m: matches to shuffle in place.
    void randomizeMatches(Matches& m);

    // Computes homography between two images given matching pixels.
    // match *matches: matching points between images.
    // returns: matrix representing homography H that maps image a to image b.
    Matd computeHomography(Matches const& matches);

    // Perform RANdom SAmple Consensus to calculate homography for noisy matches.
    // match *m: set of matches.
    // float thresh: inlier/outlier distance threshold.
    // int k: number of iterations to run.
    // int cutoff: inlier cutoff to exit early.
    // returns: matrix representing most common homography between matches.
    Matd RANSAC(Matches& m, float thresh, int k, int cutoff);

    // Apply a projective transformation to a point.
    // matrix H: homography to project point.
    // point p: point to project.
    // returns: point projected using the homography.
    Vector2<float> projectPoint(Mat const& H, Vector2<float> const& p);
    Vector2<float> projectPoint(Matd const& H, Vector2<float> const& p);

    // Perform non-max supression on an image of feature responses.
    // image im: 1-channel image of feature responses.
    // int w: distance to look for larger responses.
    // dst - output: image with only local-maxima responses within w pixels.
    void nonMaxSupression(Mat const& im, Mat& dst, int w);

    // Gets the minimum eigen value of a 2x2 matrix
    // [a b]
    // [c d]
    float minEigenValue2x2(float a, float b, float c, float d);
    float minEigenValue2x2(Mat const& m);

    // Calculate the structure matrix of an image.
    // image im: the input image.
    // float sigma: std dev. to use for weighted sum.
    // image s: output - structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2, third channel is IxIy.
    void harrisStructureMatrix(Mat const& im, Mat& S,float sigma);

    // Estimate the cornerness of each pixel given a structure matrix S.
    // image S: structure matrix for an image.
    // image r: output - a response map of cornerness calculations.
    void harrisCornernessResponse(Mat const& S, Mat& R);

    // Estimate the cornerness of each pixel given a structure matrix S.
    // image S: structure matrix for an image.
    // image r: output - a response map of cornerness calculations.
    void shiTomasiCornernessResponse(Mat const& S, Mat& R);

    // Perform harris corner detection and extract features from the corners.
    // image im: input image.
    // float sigma: std. dev for harris.
    // float thresh: threshold for cornerness.
    // int nms: distance to look for local-maxes in response map.
    // shi_tomasi : use shi tomasi variant
    // returns: array of descriptors of the corners in the image.
    Descriptors harrisCornerDetector(Mat const& im, float sigma, float thresh, int nms, bool shi_tomasi = true);

} // namespace vs
