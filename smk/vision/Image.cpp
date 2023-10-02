#include "Image.hpp"
#include "Optimization.hpp"
#include "Features.hpp"
#include "../math/Mathematics.hpp"
#include "../File.hpp"

#include <iostream>
#include <cassert>

#ifdef INCLUDE_STB_IMAGE
    #define STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image.h"
    #include "stb_image_write.h"
#endif //INCLUDE_STB_IMAGE

namespace smk
{

    Mat loadImage(str32 path, int channels)
    {
        path = convertFileNameToNativeStyle(path);

        int w, h, c;
        unsigned char *data = stbi_load(str32To8(path).c_str(), &w, &h, &c, 0);
        if (!data) {
            std::cerr << "Cannot load image \"" << str32To8(path) << "\" - " << stbi_failure_reason() << std::endl;
            return Mat();
        }

        if (channels <= 0)
        {
            channels = c;
        }

        Mat im(w, h, channels);
        for (int k = 0; k < channels; ++k)
        {
            for (int j = 0; j < h; ++j)
            {
                for (int i = 0; i < w; ++i)
                {
                    int dst_index = i + w * j + w * h * k;
                    int src_index = k + c * i + c * w * j;
                    im.data[dst_index] = float(data[src_index]) / 255.0f;
                }
            }
        }

        free(data);
        return im;
    }

    bool saveImage(str32 path, Mat const &im)
    {
        path = convertFileNameToNativeStyle(path);
        str32 ext = getFileExtension(path);
        
        std::string path8 = str32To8(path);

        int quality = 80;

        unsigned char *data = static_cast<unsigned char *>(calloc(size_t(im.w * im.h * im.c), sizeof(char)));

        for (int k = 0; k < im.c; ++k)
        {
            for (int i = 0; i < im.w * im.h; ++i)
            {
                data[i * im.c + k] = static_cast<unsigned char>(255 * im.data[i + k * im.w * im.h]);
            }
        }

        bool ok = false;
        if (ext == U"png")
        {
            ok = stbi_write_png(path8.c_str(), im.w, im.h, im.c, data, im.w * im.c) > 0;
        }
        else if (ext == U"tga")
        {
            ok = stbi_write_tga(path8.c_str(), im.w, im.h, im.c, data) > 0;
        }
        else if (ext == U"bmp")
        {
            ok = stbi_write_bmp(path8.c_str(), im.w, im.h, im.c, data) > 0;
        }
        else if (ext == U"jpg")
        {
            ok = stbi_write_jpg(path8.c_str(), im.w, im.h, im.c, data, quality) > 0;
        }

        free(data);

        return ok;
    }

    Mat loadImage(str8 path, int channels) { return loadImage(str8To32(path), channels); }
    bool saveImage(str8 path, Mat const &im) { return saveImage(str8To32(path), im); }


    void rgb2gray(Mat const &src, Mat &dst)
    {
        assert(src.w >= 0 && src.h >= 0 && ((src.c == 3) || (src.c == 4)));
        dst.reshape(src.w, src.h, 1);
        dst.zero();

        static float scale[] = {0.299f, 0.587f, 0.114f};
        for (int k = 0; k < 3; ++k)
        {
            for (int j = 0; j < src.h; ++j)
            {
                for (int i = 0; i < src.w; ++i)
                {
                    dst.data[i + dst.w * j] += scale[k] * src.get(i, j, k);
                }
            }
        }
    }

    void rgb2hsv(Mat const &src, Mat &dst)
    {
        assert(src.w >= 0 && src.h >= 0 && src.c == 3);
        dst.reshape(src.w, src.h, 3);

        int i, j;
        float r, g, b;
        float h, s, v;
        for (j = 0; j < src.h; ++j)
        {
            for (i = 0; i < src.w; ++i)
            {
                r = src.get(i, j, 0);
                g = src.get(i, j, 1);
                b = src.get(i, j, 2);
                float max = maximum(r, g, b);
                float min = minimum(r, g, b);
                float delta = max - min;
                v = max;

                if (equivalent(delta, 0.0f))
                {
                    s = 0.0f;
                    h = 0.0f;
                }
                else
                {
                    s = delta / max;

                    if (equivalent(r, max))
                    {
                        h = (g - b) / delta;
                    }
                    else if (equivalent(g, max))
                    {
                        h = 2 + (b - r) / delta;
                    }
                    else
                    {
                        h = 4 + (r - g) / delta;
                    }

                    if (h < 0)
                        h += 6.0;
                    h = h / 6.0f;
                }

                dst.set(i, j, 0, h);
                dst.set(i, j, 1, s);
                dst.set(i, j, 2, v);
            }
        }
    }

    void hsv2rgb(Mat const &src, Mat &dst)
    {
        assert(src.w >= 0 && src.h >= 0 && src.c == 3);
        dst.reshape(src.w, src.h, 3);

        int i, j;
        float r, g, b;
        float h, s, v;
        float f, p, q, t;
        for (j = 0; j < src.h; ++j)
        {
            for (i = 0; i < src.w; ++i)
            {
                h = src.get(i, j, 0) * 6.0f;
                s = src.get(i, j, 1);
                v = src.get(i, j, 2);
                if (equivalent(s, 0.0f))
                {
                    r = g = b = v;
                }
                else
                {
                    int index = int(floor(h));
                    f = h - index;
                    p = v * (1 - s);
                    q = v * (1 - s * f);
                    t = v * (1 - s * (1 - f));
                    if (index == 0)
                    {
                        r = v;
                        g = t;
                        b = p;
                    }
                    else if (index == 1)
                    {
                        r = q;
                        g = v;
                        b = p;
                    }
                    else if (index == 2)
                    {
                        r = p;
                        g = v;
                        b = t;
                    }
                    else if (index == 3)
                    {
                        r = p;
                        g = q;
                        b = v;
                    }
                    else if (index == 4)
                    {
                        r = t;
                        g = p;
                        b = v;
                    }
                    else
                    {
                        r = v;
                        g = p;
                        b = q;
                    }
                }
                dst.set(i, j, 0, r);
                dst.set(i, j, 1, g);
                dst.set(i, j, 2, b);
            }
        }
    }

    Mat rgb2gray(Mat const &src)
    {
        Mat dst;
        rgb2gray(src, dst);
        return dst;
    }

    Mat rgb2hsv(Mat const &src)
    {
        Mat dst;
        rgb2hsv(src, dst);
        return dst;
    }

    void rgb2hsvInplace(Mat &inplace)
    {
        rgb2hsv(inplace, inplace);
    }

    Mat hsv2rgb(Mat const &src)
    {
        Mat dst;
        hsv2rgb(src, dst);
        return dst;
    }

    void hsv2rgbInplace(Mat &inplace)
    {
        hsv2rgb(inplace, inplace);
    }

    void rgb2bgr(Mat const &src, Mat &dst)
    {
        assert(src.c == 3);
        dst.reshape(src.w, src.h, 3);

        for (int i = 0; i < src.w * src.h; ++i)
        {
            float r = src.data[i + src.w * src.h * 0];
            float g = src.data[i + src.w * src.h * 1];
            float b = src.data[i + src.w * src.h * 2];

            dst.data[i + src.w * src.h * 0] = b;
            dst.data[i + src.w * src.h * 1] = g;
            dst.data[i + src.w * src.h * 2] = r;
        }
    }

    Mat rgb2bgr(Mat const &src)
    {
        Mat dst;
        rgb2bgr(src, dst);
        return dst;
    }

    void rgb2bgrInplace(Mat &inplace)
    {
        rgb2bgr(inplace, inplace);
    }


    Mat::Type thresholdOtsu(const Mat &src, Mat &dst, const ThresholdMode mode, const Mat::Type max)
    {
        assert(src.c == 1);
        dst.reshape(src.w, src.h, src.c);

        int const bins = 256;
        double const size = src.size();

        double histogram[bins];
        memset(histogram, 0, bins * sizeof(double));

        float const bin_size = 1.0f / bins;

        // compute the histogram
        for (int i = 0; i < src.channelSize(); ++i) {
            int bin = clampTo(int(src.data[i] / bin_size), 0, bins - 1);
            histogram[bin] += 1.0;
        }

        for(int i = 0; i <= 255; i++) {
            histogram[i] /= size;
        }

        // compute probabilities
        //
        // probability = class probability
        // mean = class mean
        // between = between class variance
        //
        double probability[bins];
        double mean[bins];
        double max_between = 0.0;
        double between[bins];
        int threshold_bin = 0;
        memset(probability, 0, bins * sizeof(double));
        memset(mean, 0, bins * sizeof(double));
        memset(between, 0, bins * sizeof(double));

        probability[0] = histogram[0];
        for (size_t i = 1; i < bins; ++i) {
            probability[i] = probability[i - 1] + histogram[i];
            mean[i] = mean[i - 1] + i * histogram[i];
        }

        for(size_t i = 0; i < bins; i++) {

            if(probability[i] > 0.0 && probability[i] < 1.0)
                between[i] = pow(mean[bins - 1] * probability[i] - mean[i], 2) / (probability[i] * (1.0 - probability[i]));
            else
                between[i] = 0.0;

            if(between[i] > max_between) {
                max_between = between[i];
                threshold_bin = int(i);
            }
        }

        float value = clampTo(threshold_bin + 1, 0, bins - 1) * bin_size;
        return threshold(src, dst, mode, value, max);
    }

    Mat::Type threshold(const Mat &src, Mat &dst, const ThresholdMode mode, const Mat::Type value, const Mat::Type max)
    {
        assert(src.c == 1);

        dst.reshape(src.w, src.h, src.c);

        if (mode == ThresholdMode::Binary) {

            for (int i = 0; i < src.channelSize(); ++i)
                dst.data[i] = src.data[i] > value ? max : 0;

        } else if (mode == ThresholdMode::BinaryInverted) {

            for (int i = 0; i < src.channelSize(); ++i)
                dst.data[i] = src.data[i] > value ? 0 : max;

        } else if (mode == ThresholdMode::Truncate) {

            for (int i = 0; i < src.channelSize(); ++i)
                dst.data[i] = src.data[i] > value ? max : src.data[i];

        } else if (mode == ThresholdMode::ToZero) {

            for (int i = 0; i < src.channelSize(); ++i)
                dst.data[i] = src.data[i] > value ? src.data[i] : 0;

        } else if (mode == ThresholdMode::ToZeroInverted) {

            for (int i = 0; i < src.channelSize(); ++i)
                dst.data[i] = src.data[i] > value ? 0 : src.data[i];
        }

        return value;
    }

    float interpolateNN(Mat const &im, float x, float y, int c)
    {
        const int ix = int(floorf(x));
        const int iy = int(floorf(y));
        return im.get(ix, iy, c);
    }

    float interpolateBL(Mat const &im, float x, float y, int c)
    {
        // make sure we fall between the pixel center points
        x -= 0.5f;
        y -= 0.5f;

        const int ix = int(floorf(x));
        const int iy = int(floorf(y));

        const float v1 = im.getClamp(ix + 0, iy + 0, c);
        const float v2 = im.getClamp(ix + 1, iy + 0, c);
        const float v3 = im.getClamp(ix + 0, iy + 1, c);
        const float v4 = im.getClamp(ix + 1, iy + 1, c);

        const float d1 = x - ix;
        const float d2 = 1.0f - d1;
        const float d3 = y - iy;
        const float d4 = 1.0f - d3;

        const float q1 = v1 * d2 + v2 * d1;
        const float q2 = v3 * d2 + v4 * d1;
        const float q = q1 * d4 + q2 * d3;

        return q;
    }

    void resize(Mat const &src, Mat &dst, int nw, int nh, const ResizeMode mode)
    {
        dst.reshape(nw, nh, src.c);

        float (*interpolate)(Mat const &, float, float, int) = (mode == Bilinear) ? interpolateBL : interpolateNN;

        float x_ratio = float(src.w) / float(dst.w);
        float y_ratio = float(src.h) / float(dst.h);
        for (int k = 0; k < dst.c; ++k)
        {
            for (int y = 0; y < nh; ++y)
            {
                for (int x = 0; x < nw; ++x)
                {
                    float px = (x + 0.5f) * x_ratio;
                    float py = (y + 0.5f) * y_ratio;

                    float value = interpolate(src, px, py, k);
                    dst.set(x, y, k, value);
                }
            }
        }
    }

    Mat resize(Mat const &src, int nw, int nh, const ResizeMode mode)
    {
        Mat dst;
        resize(src, dst, nw, nh, mode);
        return dst;
    }

    Mat cylindricalProject(Mat const &im, float f)
    {
        Mat out(im.w, im.h, im.c);

        float center_x = out.w / 2.0f;
        float center_y = out.h / 2.0f;

        for (int k = 0; k < out.c; ++k)
            for (int y = 0; y < out.h; ++y)
                for (int x = 0; x < out.w; ++x)
                {
                    // calculate angle and height
                    float angle = (x - center_x) / f;
                    float height = (y - center_y) / f;

                    // find unit cylindrical coords
                    float cylinder_x = sin(angle);
                    float cylinder_y = height;
                    float cylinder_z = cos(angle);

                    // project to image plane
                    float px = f * cylinder_x / cylinder_z + center_x;
                    float py = f * cylinder_y / cylinder_z + center_y;

                    if (px >= 0 && px < im.w && py >= 0 && py < im.h)
                    {
                        out.set(x, y, k, im.get(int(px), int(py), k));
                    }
                }

        return out;
    }

    void extractImage4points(Mat const &im, Mat &dst, const std::array<Vector2<float>, 4> &points)
    {
        using Vec = Vector2<float>;
        
        assert(im.c == dst.c);
        assert(dst.w > 0);
        assert(dst.h > 0);

        int max_x = maximum(points[0].x(), points[1].x(), points[2].x(), points[3].x());
        int max_y = maximum(points[0].y(), points[1].y(), points[2].y(), points[3].y());

        int min_x = minimum(points[0].x(), points[1].x(), points[2].x(), points[3].x());
        int min_y = minimum(points[0].y(), points[1].y(), points[2].y(), points[3].y());
        //tl, tr, bl, br
        std::array<Vec, 4> corners = {
            Vec(min_x, min_y), Vec(max_x, min_y),
            Vec(min_x, max_y), Vec(max_x, max_y)
        };


        // compute a distance matrix
        Mat distances(4, 4);
        size_t d_size = 4;

        for (size_t y = 0; y < d_size; ++y)
            for (size_t x = 0; x < d_size; ++x)
            {
                float dx = corners[y].x() - points[x].x();
                float dy = corners[y].y() - points[x].y();
                distances(int(y), int(x)) = dx * dx + dy * dy;
            }

        // convert to integer values
        // since we will use max cost, flip sign
        double max_distance = double(distances.max());
        int max_integer = std::numeric_limits<int>::max();

        Matl idistances(distances.w, distances.h);
        for (int y = 0; y < idistances.h; ++y)
            for (int x = 0; x < idistances.w; ++x)
            {
                double distance = double(distances(y, x)) / max_distance;
                distance *= double(max_integer);
                idistances(y, x) = Matl::Type(-std::round(distance));
            }

        // find the assignment of corners to pts
        Assignment assignment = assignmentMaxCost(idistances);

        // from_points tl, tr, bl, br
        Matches matches(4);
        matches[0].p = Vec(0.0f, 0.0f);
        matches[1].p = Vec(dst.w - 1, 0.0f);
        matches[2].p = Vec(0.0f, dst.h - 1);
        matches[3].p = Vec(dst.w - 1, dst.h - 1);

        // to_points tl, tr, bl, br
        matches[0].q = points[size_t(assignment[0])];
        matches[1].q = points[size_t(assignment[1])];
        matches[2].q = points[size_t(assignment[2])];
        matches[3].q = points[size_t(assignment[3])];


        Matd H = computeHomography(matches);
        if (H.size() == 0) {
            return;
        }

        // warp image
        for (int k = 0; k < dst.c; ++k)
            for (int y = 0; y < dst.h; ++y)
                for (int x = 0; x < dst.w; ++x)
                {
                    Vec p = projectPoint(H, Vec(x, y));
                    int px = int(p.x());
                    int py = int(p.y());
                    if (px < 0 || px >= im.w || py < 0 || py >= im.h)
                        continue;

                    float value = interpolateBL(im, px, py, k);
                    dst.set(x, y, k, value);
                }

    }

} // namespace vs
