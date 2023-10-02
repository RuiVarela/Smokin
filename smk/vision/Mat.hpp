#pragma once

#include <memory>

namespace smk
{
    template<typename T>
    class MatT
    {
    public:
        using Type = T;

        MatT();
        explicit MatT(int w, int h = 1, int c = 1);
        explicit MatT(int w, int h, int c, T* ext); // external memory pointer

        void reshape(int w, int h, int c);
        int size() const;
        int channelSize() const;
        MatT clone() const;

        template<typename TO> void convert(MatT<TO>& out);
        template<typename TO> MatT<TO> convert();

        // views
        // these are virtual view on a Mat data, they will point to the parent memory
        // thus they don't allocate new memory
        MatT channelView(int c, int count = 1);

        MatT &zero();
        MatT &fill(int c, T v); // fills a channel with v value
        MatT &fill(T v); // fills all channels with v value
        MatT &fill(MatT const& src, int src_c, int dst_c); // fills a channel with values from another mat

        // copy from
        MatT &copy(MatT const& src, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y);
        MatT &copy(MatT const& src, int dst_x, int dst_y);

        // get a row from a matrix
        MatT rowm(int const row) const;

        // channel values get/set
        T get(int x, int y = 0, int c = 0) const;
        T getClamp(int x, int y, int c) const;
        T getZero(int x, int y, int c) const;
        MatT &set(int x, int y, int c, T v);
        MatT &setClamp(int x, int y, int c, T v);

        // clamp all values between [min, max]
        MatT &clamp(int c, T min, T max);
        MatT &clamp(T min, T max);
        MatT &clamp(); // 0.0f, 1.0f
        MatT &constrain(T value); //  each pixel will be in range [-v, v]

        // Add a scalar value
        MatT &add(int c, T v);
        MatT &add(T v);
        // Add a mat
        MatT &add(MatT const &v);
        static MatT add(MatT const &a, MatT const &b);

        // Subtract a mat
        MatT &sub(MatT const &v);
        static MatT sub(MatT const &a, MatT const &b);

        // Multiply by a scalar
        MatT &mult(int c, T v);
        MatT &mult(T v);

        // transpose
        MatT transpose() const;

        T sum(int c = 0); // sums all values in a channel
        T max(int c = 0); // max value in a channel
        T min(int c = 0); // min value in a channel
        void minNmax(int c, T& minv, T& maxv); // min and max value in a channel

        // normalizes a channels using l1norm
        MatT &l1Normalize(int c);
        MatT &l1Normalize();

        // normalizes using (min-max normalization) https://en.wikipedia.org/wiki/Feature_scaling
        MatT &featureNormalize(int c);
        MatT &featureNormalize();


        //
        // Math Matrix Code
        // Rows,Cols
        //
        const T &operator()(int const row, int const col) const;
        T &operator()(int const row, int const col);

        MatT &setIdentity();

        MatT augment() const;
        MatT invert() const;

        static MatT makeIdentity(int rows, int cols);
        static MatT makeIdentity3x3();
        static MatT makeTranslation3x3(T dx, T dy);

        // Matrix-Matrix multiplication
        // p = a * b
        static void mmult(const MatT &a, const MatT &b, MatT& p);
        static MatT mmult(const MatT &a, const MatT &b);

        // Matrix-Vector multiplication
        // vp = ma * vb
        static void vmult(const MatT &a, const MatT &b, MatT& p);
        static MatT vmult(const MatT &a, const MatT &b);

        // linear least squares solver
        // https://textbooks.math.gatech.edu/ila/least-squares.html
        static MatT llsSolve(MatT const& M, MatT const& b);

        int w;    // width
        int h;    // height
        int c;    // channels;
        T *data;
    private:
        std::shared_ptr<T> shared_data;
    };

    using Mat = MatT<float>;
    using Matd = MatT<double>;
    using Matl = MatT<long long>;
}
