# Smokin
This a collection of lighweight libraries to help out on development of other projects.

## [Text](https://github.com/RuiVarela/Smokin/blob/main/smk/Text.hpp)
- All functions work on ascii strings or utf32 codepoint strings
- Simple helpers (trim, startsWith, endsWith)
- unicode transforms
- glob matching
- levenshtein distance
- [a lot of tests](https://github.com/RuiVarela/Smokin/blob/main/test/Text.cpp)

## [File](https://github.com/RuiVarela/Smokin/blob/main/smk/File.hpp)
- File handling supporting utf32 filenames
- utf8 file loading
- Basic filename handlers
- cpp binary dumper
- This library requires Text lib

## [Log](https://github.com/RuiVarela/Smokin/blob/main/smk/Log.hpp)
- Simple Logging system
- Threaded
- Thread safe

## [Easing](https://github.com/RuiVarela/Smokin/blob/main/smk/Easing.hpp)
- Easing functions based on [easings.net](https://easings.net/)

## [Mathematics](https://github.com/RuiVarela/Smokin/tree/main/smk/math)
- Core math helpers
- Vector2, Vector3, Vector4
- Matrix44
- Quaternion
- BoundingVolume
- Running Average calculator
- RMS calculator

## [Timer](https://github.com/RuiVarela/Smokin/blob/main/smk/Timer.hpp)
- easy clock query
- ElapsedTimer timekeeping
- DurationProbe for debugging

## [UnitTest](https://github.com/RuiVarela/Smokin/blob/main/smk/UnitTest.hpp)
- Super simple unit test library
- Used on smoking to test it's internal code. but it can be easely used standalone.
- Test cases are automatically registered.
- Dumb [runner](https://github.com/RuiVarela/Smokin/blob/main/test/Tester.cpp)

## [Vision](https://github.com/RuiVarela/Smokin/tree/main/smk/vision)
- Planar image represention using floats
- Basic Mat structure with simple usage
- Nearest Neighbor and Bilinear interpolation resize
- Color Conversion (rgb <-> hsv)
- Convolutions
- Filters (Gaussian, sobel, etc)
- Harris Corner detector
- Shi-Tomasi Corner detector
- Homography calculation
- RANSAC fitting example for noisy matched features
- Lukas Kanade optical flow calculation
- Canny Edge Detector
- Max Cost Assigment
- Image rectangle extraction and warping
- Thresholding (Binary, BinaryInverted, Truncate, ToZero, ToZeroInverted) with otsu

## [Audio](https://github.com/RuiVarela/Smokin/blob/main/smk/audio)
- Circular buffer suitable for audio 
- Wav Reader / Writer
- Wav Recorder

## [Threading](https://github.com/RuiVarela/Smokin/tree/main/smk/threading)
- Barrier primitive
- WaitEvent primitive
- Thread abstraction on a Worker class
- WorkQueue for task scheduling

## Development and running tests
```bash
git git@github.com:RuiVarela/Smokin.git
cd Smokin

mkdir build
cd build

cmake .. -DBUILD_UNIT_TESTS=On
cmake --build .
./Debug/test
```

# Credits
- [rui old code](https://github.com/RuiVarela)
- [tsoding rants](https://github.com/tsoding)
- [Joseph Redmon CSE 455 homeworks](https://github.com/pjreddie/vision-hw0)
