#include "Opticalflow.hpp"
#include "Filter.hpp"
#include "Image.hpp"
#include "Features.hpp"

#include <cassert>

namespace smk
{

    void makeIntegralImage(const Mat &im, Mat &out)
    {
        out.reshape(im.w, im.h, im.c);

        for (int k = 0; k != im.c; ++k)
            for (int y = 0; y != im.h; ++y)
                for (int x = 0; x != im.w; ++x)
                {
                    float v = im.get(x, y, k);

                    if (y > 0)
                        v += out.get(x, y - 1, k);

                    if (x > 0)
                        v += out.get(x - 1, y, k);

                    if (x > 0 && y > 0)
                        v -= out.get(x - 1, y - 1, k);

                    out.set(x, y, k, v);
                }
    }


    void getIntegralImageRegion(const Mat &im, int c, int l, int t, int r, int b, float &sum, int &count)
    {
        if (l < 0)
            l = 0;

        if (t < 0)
            t = 0;

        if (r >= im.w)
            r = im.w - 1;

        if (b >= im.h)
            b = im.h - 1;

        float A = 0;
        float B = 0;
        float C = 0;
        float D = im.get(r, b, c);

        if (l - 1 >= 0 && t - 1 >= 0) {
            A = im.get(l - 1, t - 1, c);
            B = im.get(r, t - 1, c);
            C = im.get(l - 1, b, c);
        } else if (l - 1 >= 0) {
            C = im.get(l - 1, b, c);
        } else if (t - 1 >= 0) {
            B = im.get(r, t - 1, c);
        }

        sum = D + A - B - C;
        count = (r - l + 1) * (b - t + 1);
    }


    void boxfilterIntegralImage(const Mat &im, int smooth, Mat &out)
    {
        out.reshape(im.w, im.h, im.c);

        int offset = int(smooth / 2);
        int wi = im.w - 1;
        int hi = im.h - 1;

        for (int k = 0; k != im.c; ++k)
            for (int y = 0; y != im.h; ++y)
                for (int x = 0; x != im.w; ++x)
                {
                    int l = x - offset;
                    int t = y - offset;
                    int r = x + offset;
                    int b = y + offset;

                    float sum;
                    int count;
                    getIntegralImageRegion(im, k, l, t, r, b, sum, count);
                    out.set(x, y, k, sum / float(count));
                }
    }

    void LucasKanade::timeStructureMatrix(const Mat &im, const Mat &prev, int smooth, Mat &S)
    {
        // returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
        //          3rd channel is IxIy, 4th channel is IxIt, 5th channel is IyIt.

        S.reshape(im.w, im.h, 5);

        m_I.reshape(im.w, im.h, 5);
        Mat IxIx = m_I.channelView(0);
        Mat IyIy = m_I.channelView(1);
        Mat IxIy = m_I.channelView(2);
        Mat IxIt = m_I.channelView(3);
        Mat IyIt = m_I.channelView(4);
        gradient(im, IxIx, IyIy);

        float x, y, t;
        for (int i = 0; i != im.size(); ++i)
        {
            x = IxIx.data[i];
            y = IyIy.data[i];
            t = im.data[i] - prev.data[i];

            IxIy.data[i] = x * y;
            IxIx.data[i] = x * x;
            IyIy.data[i] = y * y;
            IxIt.data[i] = x * t;
            IyIt.data[i] = y * t;
        }

        makeIntegralImage(m_I, m_Ii);
        boxfilterIntegralImage(m_Ii, smooth, S);
    }

    void LucasKanade::velocityImage(const Mat &S, int stride, Mat &v)
    {
        float eigen_threshold = 0.0002f;

        v.reshape(S.w/stride, S.h/stride, 3);
        v.zero();

        Mat A(2,2);
        Mat B(1, 2);
        Mat P(1, 2);

        for (int y = (stride - 1) / 2; y < S.h; y += stride)
        {
            int ty = y / stride;
            if (ty >= v.h)
                continue;

            for (int x = (stride - 1) / 2; x < S.w; x += stride)
            {

                int tx = x / stride;
                if (tx >= v.w)
                    break;

                A(0, 0) = S.get(x, y, 0); // Ixx
                A(0, 1) = S.get(x, y, 2); // Ixy
                A(1, 0) = S.get(x, y, 2); // Ixy
                A(1, 1) = S.get(x, y, 1); // Iyy

                B(0, 0) = -S.get(x, y, 3); //Ixt
                B(1, 0) = -S.get(x, y, 4); //Iyt

                // check for invertability
                if (minEigenValue2x2(A) > eigen_threshold)
                {
                    Mat Ai = A.invert();
                    if (Ai.size() == 0)
                        continue;

                    Mat::vmult(Ai, B, P);
                    v.set(tx, ty, 0, P(0, 0));
                    v.set(tx, ty, 1, P(1, 0));
                }
            }
        }
    }

    void LucasKanade::opticalflow(const Mat &im, const Mat &prev, int smooth, int stride, Mat &vs)
    {
        assert(im.w == prev.w && im.h == prev.h);

        if (im.c == 1) 
            m_curr_gray = im;
        else    
            rgb2gray(im, m_curr_gray); 

        if (prev.c == 1) 
            m_prev_gray = prev;
        else    
            rgb2gray(prev, m_prev_gray);        
        
        timeStructureMatrix(m_curr_gray, m_prev_gray, smooth, m_S);
        velocityImage(m_S, stride, m_V);

        m_V.constrain(6.0f);
        smoothImage(m_V, vs, 2.0);
    }

} 
