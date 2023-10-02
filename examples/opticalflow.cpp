#include <vision/Mat.hpp>
#include <vision/Features.hpp>
#include <vision/Image.hpp>
#include <vision/Drawing.hpp>
#include <vision/Opticalflow.hpp>

#include <CommandLine.hpp>

using namespace smk;

void addImage(std::string const& prefix, int index, std::string const& suffix, std::vector<Mat>& stream) {
    std::string file = prefix + to_str8(index) + suffix;
    Mat loaded = loadImage(file);
    
    if (loaded.size() != 0) {
        stream.push_back(loaded);
    }
}

int main(int argc, char **argv)
{
    int smooth = findArgInt(argc, argv, "smooth", 15);
    int stride = findArgInt(argc, argv, "stride", 4);
    int div = findArgInt(argc, argv, "div", 4);
    
    std::string prefix = "oflow_stream";
    std::string suffix = ".png";
    std::vector<Mat> stream;
    addImage(prefix, 0, suffix, stream);
    addImage(prefix, 1, suffix, stream);
    addImage(prefix, 2, suffix, stream);
    addImage(prefix, 3, suffix, stream);
    addImage(prefix, 4, suffix, stream);
    addImage(prefix, 5, suffix, stream);
    
    int streamIndex = 0;
    int streamCount = stream.size();
    
    
    if (stream.size() < 3) {
        return 2;
    }
    
    
    Mat prev, prev_c;
    Mat im, im_c;

    prev = stream[streamIndex++];
    resize(prev, prev_c, prev.w / div, prev.h / div);

    im = stream[streamIndex++];
    resize(im, im_c, im.w / div, im.h / div);

    LucasKanade lk;
    Mat v;

    while (streamIndex < streamCount) {
        lk.opticalflow(im_c, prev_c, smooth, stride, v);
        drawFlow(im, v, smooth * div);
        
        saveImage("flow_" + to_str8(streamIndex), im);

        std::swap(prev, im);
        std::swap(prev_c, im_c);

        im = stream[streamIndex++];
        resize(im, im_c, im.w / div, im.h / div);
    }
    return 0;
}
