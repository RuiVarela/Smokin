#include "Features.hpp"
#include "Filter.hpp"
#include "Image.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace smk
{
    void Descriptor::reshape(int size)
    {
        if (n == size)
            return;

        shared_data.reset();
        data = nullptr;
        n = 0;

        if (size > 0)
        {
            n = size;
            data = static_cast<float*>(calloc(size_t(size), sizeof(float)));
            if (data)
            {
                shared_data = std::shared_ptr<float>(data, free);
            }
        }
    }

    Descriptor Descriptor::describe(const Mat &im, int i)
    {
        int x = i % im.w;
        int y = i / im.w;

        int w = 5;
        Descriptor d;
        d.p.x() = float(x);
        d.p.y() = float(y);
        d.reshape(w * w * im.c);

         float mean = 0.0f;
         for (int c = 0; c < im.c; ++c)
             for (int dx = -w / 2; dx < (w + 1) / 2; ++dx)
                 for (int dy = -w / 2; dy < (w + 1) / 2; ++dy)
                     mean = im.getClamp(x + dx, y + dy, c);

         mean /= float(w * w * im.c);

         int count = 0;
         for (int c = 0; c < im.c; ++c)
             for (int dx = -w / 2; dx < (w + 1) / 2; ++dx)
                 for (int dy = -w / 2; dy < (w + 1) / 2; ++dy)
                     d.data[count++] = mean - im.getClamp(x + dx, y + dy, c);
    /*
        int count = 0;
        // If you want you can experiment with other descriptors
        // This subtracts the central value from neighbors
        // to compensate some for exposure/lighting changes.
        for (int c = 0; c < im.c; ++c)
        {
            float cval = im.get(x, y, c);
            for (int dx = -w / 2; dx < (w + 1) / 2; ++dx)
                for (int dy = -w / 2; dy < (w + 1) / 2; ++dy)
                    d.data[count++] = cval - im.getClamp(x + dx, y + dy, c);
        }
    */
        return d;
    }

    float Descriptor::distance(const Descriptor &a, const Descriptor &b)
    {

        // Calculates L1 distance between to Descriptors
        // calculates the l1 distance on the floating point arrays.
        // Minkowski distance between two points of order 1
        // https://en.wikipedia.org/wiki/Minkowski_distance
        // Descriptor *a, *b: to compare.
        // returns: l1 distance between them (sum of absolute differences).
        assert(a.n == b.n && a.n > 0);

        float sum = 0;
        for (int i = 0; i != a.n; ++i)
            sum += fabsf(a.data[i] - b.data[i]);

        return sum;
    }

    Matches matchDescriptors(const Descriptors &a, const Descriptors &b)
    {
        assert(!a.empty() && !b.empty());

        Matches output;

        // We will have at most a.size matches.
        for(size_t ai = 0; ai < a.size(); ++ai) {

            float best_distance = std::numeric_limits<float>::max();
            size_t best_index = 0;

            for(size_t bi = 0; bi < b.size(); ++bi) {
                float distance = Descriptor::distance(a[ai], b[bi]);
                if (distance < best_distance) {
                    best_distance = distance;
                    best_index = bi;
                }
            }

            Match m;
            m.ai = int(ai);
            m.bi = int(best_index);
            m.p = a[size_t(m.ai)].p;
            m.q = b[size_t(m.bi)].p;
            m.distance = best_distance; // <- should be the smallest L1 distance!

            output.push_back(m);
        }


        // we want matches to be injective (one-to-one).
        // Sort matches based on distance
        // Then throw out matches to the same element in b. Use seen to keep track.
        // Each point should only be a part of one match.
        // Some points will not be in a match.
        // In practice just bring good matches to front of list

        std::sort(output.begin(), output.end(), [] (Match const& a, Match const&b) { return (a.distance < b.distance); });

        Matches filtered;

        std::vector<bool> seen(b.size(), false);
        for (size_t i = 0; i != output.size(); ++i) {
            size_t bi = size_t(output[i].bi);
            if (seen[bi])
                continue;

            seen[bi] = true;
            filtered.push_back(output[i]);
        }

        return filtered;
    }

    template<typename T>
    static Vector2<float> project(const MatT<T> &H, const Vector2<float> &p) {
        assert(H.w == 3 && H.h == 3 && H.c == 1);

        MatT<T> pm(1, 3);
        pm(0,0) = T(p.x());
        pm(1,0) = T(p.y());
        pm(2,0) = 1;

        MatT<T> projected = MatT<T>::mmult(H, pm);
        return Vector2<T>(projected(0,0) / projected(2,0),
                          projected(1,0) / projected(2,0));
    }

    Vector2<float> projectPoint(const Mat &H, const Vector2<float> &p) {
        return project(H, p);
    }

    Vector2<float> projectPoint(const Matd &H, const Vector2<float> &p) {
        return project(H, p);
    }

    int modelInliers(const Matd &H, Matches &m, float thresh)
    {
        // count number of matches that are inliers
        // i.e. distance(H*p, q) < thresh
        // Also, sort the matches m so the inliers are the first 'count' elements.

        Matches inliers;
        Matches outliers;

        for (Match const &current : m)
        {
            Vector2<float> projected = projectPoint(H, current.p);
            float distance = smk::distance(current.q, projected);
            if (distance < thresh)
                inliers.push_back(current);
            else
                outliers.push_back(current);
        }

        m.assign(inliers.begin(), inliers.end());
        std::copy(outliers.begin(), outliers.end(), std::back_inserter(m));

        return int(inliers.size());
    }

    void randomizeMatches(Matches& m) {
        // Fisher-Yates to shuffle the array.
        for (size_t i = 0; i != m.size() - 2; ++i){
            size_t j = i + size_t(rand() % int(m.size() - i));
            std::swap(m[i], m[j]);
        }
    }

    //https://math.stackexchange.com/questions/494238/how-to-compute-homography-matrix-h-from-corresponding-points-2d-2d-planar-homog
    Matd computeHomography(Matches const &matches)
    {
        size_t n = matches.size();
        Matd M(8, int(n * 2));
        Matd b(1, int(n * 2));

        // fill in the matrices M and b.
        for (size_t i = 0; i < n; ++i)
        {
            double x = double(matches[i].p.x());
            double xp = double(matches[i].q.x());

            double y = double(matches[i].p.y());
            double yp = double(matches[i].q.y());

            int r = int(i * 2);
            M(r, 0) = x;
            M(r, 1) = y;
            M(r, 2) = 1;
            M(r, 3) = 0;
            M(r, 4) = 0;
            M(r, 5) = 0;
            M(r, 6) = -x * xp;
            M(r, 7) = -y * xp;
            b(r, 0) = xp;

            r++;

            M(r, 0) = 0;
            M(r, 1) = 0;
            M(r, 2) = 0;
            M(r, 3) = x;
            M(r, 4) = y;
            M(r, 5) = 1;
            M(r, 6) = -x * yp;
            M(r, 7) = -y * yp;
            b(r, 0) = yp;
        }

        Matd a = Matd::llsSolve(M, b);

        // If a solution can't be found, return empty matrix;
        if (a.size() == 0)
            return a;

        // fill in the homography H based on the result in a.
        Matd H(3, 3);

        H(0,0) = a(0,0);
        H(0,1) = a(1,0);
        H(0,2) = a(2,0);
        
        H(1,0) = a(3,0);
        H(1,1) = a(4,0);
        H(1,2) = a(5,0);

        H(2,0) = a(6,0);
        H(2,1) = a(7,0);
        H(2,2) = 1.0;

        return H;
    }

    Matd RANSAC(Matches &m, float thresh, int k, int cutoff)
    {
        assert(m.size() > 4);

        // RANSAC algorithm.
        // for k iterations:
        //     shuffle the matches
        //     compute a homography with a few matches (how many??)
        //     if new homography is better than old (how can you tell?):
        //         compute updated homography using all inliers
        //         remember it and how good it is
        //         if it's better than the cutoff:
        //             return it immediately
        // if we get to the end return the best homography

        //n – minimum number of data points required to estimate model parameters
        const int n = 4;
        int best = 0;
        Matd Hb;
        Matches subset;

        int current_iteration = 0;
        while (current_iteration < k) {
            current_iteration++;

            randomizeMatches(m);
            subset.assign(m.begin(), m.begin() + n);
            Matd H = computeHomography(subset);
            if (H.size() == 0) {
                //std::cerr << "Homography is empty" << std::endl;
                continue;
            }

            int inliers = modelInliers(H, m, thresh);
            if (inliers <= best)
                continue;

            subset.assign(m.begin(), m.begin() + inliers);
            H = computeHomography(subset);
            if (H.size() == 0) {
                std::cerr << "Homography is empty on full inliers" << std::endl;
                continue;
            }

            best = inliers;
            Hb = H;

            if (best > cutoff) {
                break;
            }
        }

       // std::cout << "Matches: " << m.size() << " Inliers: " << best << " Iters: " << current_iteration<<std::endl;

        return Hb;
    }

    void nonMaxSupression(Mat const &im, Mat &dst, int w)
    {
        // image r = copy_image(im);
        // perform NMS on the response map.
        // for every pixel in the image:
        //     for neighbors within w:
        //         if neighbor response greater than pixel response:
        //             set response to be very low (I use -999999)

        dst.reshape(im.w, im.h, 1);
        dst.fill(im, 0, 0);

        const float low_response = std::numeric_limits<float>::min();
        const int c = 0;

        for (int y = 0; y != im.h; ++y)
            for (int x = 0; x != im.w; ++x)
            {
                float value = im.getClamp(x, y, c);
                for (int ky = -w; ky <= w && (value > low_response); ++ky)
                    for (int kx = -w; kx <= w && (value > low_response); ++kx)
                        if (im.getClamp(x + kx, y + ky, c) > value)
                        {
                            dst.set(x, y, c, low_response);
                            value = low_response;
                        }
            }
    }

    void harrisStructureMatrix(Mat const &im, Mat &S, float sigma)
    {
        int size = im.w * im.h;

        Mat I(im.w, im.h, 3);
        Mat IxIx = I.channelView(0);
        Mat IyIy = I.channelView(1);
        Mat IxIy = I.channelView(2);
        gradient(im, IxIx, IyIy);

        float x, y;
        for (int i = 0; i != size; ++i)
        {
            x = IxIx.data[i];
            y = IyIy.data[i];

            IxIy.data[i] = x * y;
            IxIx.data[i] = x * x;
            IyIy.data[i] = y * y;
        }

        S.reshape(im.w, im.h, 3);
        smoothImage(I, S, sigma);
    }

    void harrisCornernessResponse(Mat const &s, Mat &R)
    {
        float const alpha = 0.06f;

        // fill in R, "cornerness" for each pixel using the structure matrix.
        // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
        //
        // H() [IxIx, IxIy]
        //     [IxIy, IyIy]

        R.reshape(s.w, s.h, 1);
        for(int i = 0; i != s.w * s.h; ++i) {
            const float xx = s.data[s.w * s.h * 0 + i];
            const float yy = s.data[s.w * s.h * 1 + i];
            const float xy = s.data[s.w * s.h * 2 + i];

            const float trace = xx + yy;
            const float det = xx * yy - xy * xy;
            R.data[i] = det - (alpha * trace * trace);
        }
    }

    float minEigenValue2x2(float a, float b, float c, float d)
    {
        float common = powf(((a + d) * (a + d))/4 - (a * d - b * c), 0.5);
        float ev_one = (a + d)/2 + common;
        float ev_two = (a + d)/2 - common;
        return (ev_one >= ev_two) ? ev_two : ev_one;
    }

    float minEigenValue2x2(Mat const& m) {
        assert(m.w == 2 && m.h == 2 && m.c == 1);
        return minEigenValue2x2(m.data[0], m.data[1], m.data[2], m.data[3]);
    }

    void shiTomasiCornernessResponse(Mat const &S, Mat &R)
    {
        float multiplier = 9.0f; // try to match harris values for thresholds

        R.reshape(S.w, S.h, 1);
        for (int i = 0; i != S.w * S.h; ++i)
        {
            const float xx = S.data[S.w * S.h * 0 + i];
            const float yy = S.data[S.w * S.h * 1 + i];
            const float xy = S.data[S.w * S.h * 2 + i];

            R.data[i] = minEigenValue2x2(xx, xy, xy, yy) * multiplier;
        }
    }

    Descriptors harrisCornerDetector(Mat const &im, float sigma, float thresh, int nms, bool shi_tomasi)
    {
        Descriptors d;
        Mat S, R;

        Mat gray = im;
        if (gray.c > 1)
            gray = rgb2gray(gray);

        // Calculate structure matrix
        harrisStructureMatrix(im, S, sigma);

        // Estimate cornerness
        if (shi_tomasi)
            shiTomasiCornernessResponse(S, R);
        else
            harrisCornernessResponse(S, R);

        // Run NMS on the responses
        nonMaxSupression(R, S, nms);

        for (int i = 0; i != S.w * S.h; ++i)
            if (S.data[i] > thresh)
                d.push_back(Descriptor::describe(gray, i));

        return d;
    }

} // namespace vs
