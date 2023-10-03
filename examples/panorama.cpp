#include <vision/Mat.hpp>
#include <vision/Features.hpp>
#include <vision/Image.hpp>
#include <vision/Drawing.hpp>

#include <CommandLine.hpp>
#include <Log.hpp>

using namespace smk;

static std::string TAG = "panorama";

static float getMin(Mat const &im, int px, int py, int c, int w) {
    float v = std::numeric_limits<float>::max();

    for (int dx = -w / 2; dx < (w + 1) / 2; ++dx)
        for (int dy = -w / 2; dy < (w + 1) / 2; ++dy)
        {
            float current = im.getClamp(px + dx, py + dy, c);
            if (current < v)
                v = current;
        }

    return v;
}

// Stitches two images together using a projective transformation.
// image a, b: images to stitch.
// matrix H: homography from image a coordinates to image b coordinates.
// returns: combined image stitched together.
static Mat combine_images(Mat const &a, Mat const &b, Matd const &H) {
    Matd Hinv = H.invert();

    // Project the corners of image b into image a coordinates.
    Vector2F c1 = projectPoint(Hinv, Vector2F(0, 0));
    Vector2F c2 = projectPoint(Hinv, Vector2F(b.w - 1, 0));
    Vector2F c3 = projectPoint(Hinv, Vector2F(0, b.h - 1));
    Vector2F c4 = projectPoint(Hinv, Vector2F(b.w - 1, b.h - 1));

    // Find top left and bottom right corners of image b warped into image a.
    Vector2F topleft, botright;
    botright.x() = maximum(c1.x(), maximum(c2.x(), maximum(c3.x(), c4.x())));
    botright.y() = maximum(c1.y(), maximum(c2.y(), maximum(c3.y(), c4.y())));
    topleft.x() = minimum(c1.x(), minimum(c2.x(), minimum(c3.x(), c4.x())));
    topleft.y() = minimum(c1.y(), minimum(c2.y(), minimum(c3.y(), c4.y())));

    // Find how big our new image should be and the offsets from image a.
    int dx = minimum(0.0f, topleft.x());
    int dy = minimum(0.0f, topleft.y());
    int w = maximum(a.w, int(botright.x())) - dx;
    int h = maximum(a.h, int(botright.y())) - dy;

    // Can disable this if you are making very big panoramas.
    // Usually this means there was an error in calculating H.
    if(w > 7000 || h > 7000){
        Log::e(TAG, "output too big, stopping");
        return a.clone();
    }

    Mat c(w, h, a.c);

    // Paste image a into the new image offset by dx and dy.
    c.copy(a, -dx, -dy);

    // Paste in image b as well.
    // You should loop over some points in the new image (which? all?)
    // and see if their projection from a coordinates to b coordinates falls
    // inside of the bounds of image b. If so, use bilinear interpolation to
    // estimate the value of b at that projection, then fill in image c.
    for (int k = 0; k < b.c; ++k)
        for (int y = topleft.y(); y < int(botright.y()); ++y)
            for (int x = topleft.x(); x < int(botright.x()); ++x) {
                Vector2F p = projectPoint(H, Vector2F(x, y));
                if (p.x() >= 0.0f && p.x() < b.w && p.y() >= 0.0f && p.y() < b.h) {

                    // this is because of the cylinder black borders
                    if (equivalent(getMin(b, p.x(), p.y(), k, 3), 0.0f))
                        continue;

                    float value = interpolateBL(b, p.x(), p.y(), k);
                    c.set(x - dx, y - dy, k, value);
                }
            }

    return c;
}

// Create a panoramam between two images.
// image a, b: images to stitch together.
// float sigma: gaussian for harris corner detector. Typical: 2
// float thresh: threshold for corner/no corner. Typical: 1-5
// int nms: window to perform nms on. Typical: 3
// float inlier_thresh: threshold for RANSAC inliers. Typical: 2-5
// int iters: number of RANSAC iterations. Typical: 1,000-50,000
// int cutoff: RANSAC inlier cutoff. Typical: 10-100
static Mat panorama_image(Mat &a, Mat &b, float sigma, float thresh, int nms, float inlier_thresh, int iters, int cutoff, bool no_match)
{
    srand(10);
    // Calculate corners and descriptors
    Descriptors ad = harrisCornerDetector(a, sigma, thresh, nms);
    Descriptors bd = harrisCornerDetector(b, sigma, thresh, nms);

    // Find matches
    Matches m;
    if (no_match) {
        for (size_t a = 0; a != ad.size(); ++a)
            for (size_t b = 0; b != bd.size(); ++b) {
                Match current;
                current.ai = int(a);
                current.bi = int(b);
                current.p = ad[size_t(current.ai)].p;
                current.q = bd[size_t(current.bi)].p;
                current.distance = 0;

                m.push_back(current);
            }
    } else {
        m = matchDescriptors(ad, bd);
    }

    // Run RANSAC to find the homography
    Matd H = RANSAC(m, inlier_thresh, iters, cutoff);

    if (H.size() == 0) {
        Log::e(TAG, "Unable to find homography");
    }

    if (false) {
        // Mark corners and matches between images
        markCorners(a, ad);
        markCorners(b, bd);
        Mat inlier_matches = drawInliers(a, b, H, m, inlier_thresh);
        saveImage("inliers.png", inlier_matches);
    }

    // Stitch the images together with the homography
    return combine_images(a, b, H);
}

//
// ./panorama draw_corners img ./data/Rainier1.png img
// ./panorama draw_matches img ./data/Rainier1.png img ./data/Rainier2.png
// ./panorama img ./data/Rainier1.png img ./data/Rainier2.png
// ./panorama thresh 10 img ./data/Rainier1.png img ./data/Rainier2.png img ./data/Rainier5.png img ./data/Rainier6.png img ./data/Rainier3.png img ./data/Rainier4.png
// ./panorama cylindrical 800 thresh 5 inlier_thresh 5 img ./data/Rainier1.png img ./data/Rainier2.png img ./data/Rainier6.png img ./data/Rainier3.png img ./data/Rainier4.png img ./data/Rainier5.png

int run(int argc, char **argv) {
    bool no_match = findArg(argc, argv, "no_match");
    float cylindrical = findArgFloat(argc, argv, "cylindrical", 0.0f);
    float sigma = findArgFloat(argc, argv, "sigma", 2.0f);
    float thresh = findArgFloat(argc, argv, "thresh", 50.0f);
    int nms = findArgInt(argc, argv, "nms", 3);
    float inlier_thresh = findArgFloat(argc, argv, "inlier_thresh", 2.0f);
    int iters = findArgInt(argc, argv, "iters", 50000);
    int cutoff = findArgInt(argc, argv, "cutoff", 30);

    std::vector<std::string> inputs;
    std::string name = findArgStr(argc, argv, "img", "");
    while (!name.empty()) {
        inputs.push_back(name);
        name = findArgStr(argc, argv, "img", "");
    }

    if (findArg(argc, argv, "draw_corners")) {
        if (inputs.empty()) {
            Log::e(TAG, "Insuficient images provided. use img param");
            return -1;
        }

        Mat a = loadImage(inputs[0]);
        drawHarrisCorners(a, sigma, thresh, nms);
        saveImage("generated.png", a);
        return 0;
    } else if (findArg(argc, argv, "draw_matches")) {
        if (inputs.size() < 2) {
            Log::e(TAG, "Insuficient images provided. use img param");
            return -1;
        }

        Mat a = loadImage(inputs[0]);
        Mat b = loadImage(inputs[1]);
        Mat out = drawMatches(a, b, sigma, thresh, nms);
        saveImage("generated.png", out);
        return 0;
    }

    if (inputs.size() < 2) {
        Log::e(TAG, "Insuficient images provided. use img param");
        return -1;
    }

    Mat current = loadImage(inputs[0], 3);
    if (cylindrical > 0.0)
        current = cylindricalProject(current, cylindrical);

    for (size_t i = 1; i != inputs.size(); ++i) {
        Log::i(TAG, "Merging " + inputs[i]);
        Mat next = loadImage(inputs[i], 3);

        if (cylindrical > 0.0)
            next = cylindricalProject(next, cylindrical);

        current = panorama_image(current, next, sigma, thresh, nms, inlier_thresh, iters, cutoff, no_match);
        saveImage("generated.png", current);
    }

    return 0;
}

int main(int argc, char **argv)
{
    Log::logger().start();
    int result = run(argc, argv);
    Log::logger().stop();
    
    return result;
}
