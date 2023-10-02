#include "Mat.hpp"
#include "../math/Mathematics.hpp"

#include <cassert>

namespace smk
{
    template <typename T>
    MatT<T>::MatT()
        : w(0), h(0), c(0), data(nullptr)
    {
    }

    template <typename T>
    MatT<T>::MatT(int w, int h, int c)
        : MatT()
    {
        reshape(w, h, c);
    }

    template <typename T>
    MatT<T>::MatT(int w, int h, int c, T *ext)
        : MatT()
    {
        assert(ext != nullptr);

        this->w = w;
        this->h = h;
        this->c = c;
        this->data = ext;
    }

    template <typename T>
    MatT<T> MatT<T>::clone() const
    {
        MatT<T> output(w, h, c);
        memcpy(output.data, data, size_t(size()) * sizeof(T));
        return output;
    }

    template <typename T>
    template <typename TO>
    void MatT<T>::convert(MatT<TO> &out)
    {
        out.reshape(w, h, c);
        for (int i = 0; i != size(); ++i)
        {
            out.data[i] = TO(data[i]);
        }
    }

    template <typename T>
    template <typename TO>
    MatT<TO> MatT<T>::convert()
    {
        MatT<TO> out;
        convert(out);
        return out;
    }

    template <typename T>
    MatT<T> MatT<T>::channelView(int c, int count)
    {
        assert(this->c > (c + count - 1));

        MatT<T> output(w, h, count, data + c * channelSize());
        // share the data across mat objets
        output.shared_data = shared_data;

        return output;
    }

    template <typename T>
    MatT<T> &MatT<T>::zero()
    {
        memset(data, 0, size_t(size()) * sizeof(T));
        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::fill(int c, T v)
    {
        assert(c >= 0 && c < this->c);

        for (int i = 0; i != w * h; ++i)
            data[c * h * w + i] = v;

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::fill(T v)
    {
        for (int k = 0; k != c; ++k)
            fill(k, v);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::fill(MatT<T> const &src, int src_c, int dst_c)
    {
        assert(src.w == w && src.h == h && src.c > src_c && c > dst_c);

        for (int i = 0; i != w * h; ++i)
            data[dst_c * w * h + i] = src.data[src_c * w * h + i];

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::copy(MatT<T> const &src, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y)
    {
        assert(src_w >= 0 && src_h >= 0 && src_w <= (src.w - src_x)  && src_h <= (src.h - src_y));
        assert(src_w <= (w - dst_x) && src_h <= (h - dst_y));

        for (int k = 0; k < src.c; ++k)
            for (int y = 0; y < src_h; ++y)
                for (int x = 0; x < src_w; ++x)
                    set(dst_x + x, dst_y + y, k, src.get(src_x + x, src_y + y, k));

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::copy(MatT<T> const &src, int dst_x, int dst_y)
    {
        return copy(src, 0, 0, src.w, src.h, dst_x, dst_y);

    }

    template <typename T>
    MatT<T> MatT<T>::rowm(int const row) const
    {
        MatT<T> single(w, 1, c);

        for (int k = 0; k != c; ++k)
            for (int x = 0; x != w; ++x)
                single.set(x, 0, k, get(x, row, k));

        return single;
    }

    template <typename T>
    T MatT<T>::get(int x, int y, int c) const
    {
        assert(x >= 0);
        assert(x < this->w);

        assert(y >= 0);
        assert(y < this->h);

        assert(c >= 0);
        assert(c < this->c);
        return data[c * h * w + y * w + x];
    }

    template <typename T>
    T MatT<T>::getClamp(int x, int y, int c) const
    {
        x = clampTo(x, 0, this->w - 1);
        y = clampTo(y, 0, this->h - 1);
        c = clampTo(c, 0, this->c - 1);
        return data[c * h * w + y * w + x];
    }

    template <typename T>
    T MatT<T>::getZero(int x, int y, int c) const
    {
        if (x < 0 || x >= this->w || y < 0 || y >= this->h || c < 0 || c >= this->c)
            return 0;

        return data[c * h * w + y * w + x];
    }

    template <typename T>
    MatT<T> &MatT<T>::set(int x, int y, int c, T v)
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(c >= 0);
        assert(x < w && y < h && c < this->c);

        data[c * h * w + y * w + x] = v;

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::setClamp(int x, int y, int c, T v)
    {
        x = clampTo(x, 0, this->w - 1);
        y = clampTo(y, 0, this->h - 1);
        c = clampTo(c, 0, this->c - 1);
        data[c * h * w + y * w + x] = v;
        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::add(int c, T v)
    {
        assert(c >= 0 && c < this->c);

        for (int i = 0; i != w * h; ++i)
            data[c * h * w + i] += v;

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::add(T v)
    {
        for (int k = 0; k != c; ++k)
            add(k, v);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::add(const MatT<T> &v)
    {
        assert(c == v.c && w == v.w && h == v.h);

        for (int i = 0; i != c * w * h; ++i)
            data[i] += v.data[i];

        return *this;
    }

    template <typename T>
    MatT<T> MatT<T>::add(const MatT<T> &a, const MatT<T> &b)
    {
        MatT<T> output = a.clone();
        output.add(b);
        return output;
    }

    template <typename T>
    MatT<T> &MatT<T>::sub(const MatT<T> &v)
    {
        assert(c == v.c && w == v.w && h == v.h);

        for (int i = 0; i != c * w * h; ++i)
            data[i] -= v.data[i];

        return *this;
    }

    template <typename T>
    MatT<T> MatT<T>::sub(const MatT<T> &a, const MatT<T> &b)
    {
        MatT<T> output = a.clone();
        output.sub(b);
        return output;
    }

    template <typename T>
    MatT<T> &MatT<T>::mult(int c, T v)
    {
        assert(c >= 0 && c < this->c);

        for (int i = 0; i != w * h; ++i)
            data[c * h * w + i] *= v;

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::mult(T v)
    {
        for (int k = 0; k != c; ++k)
            mult(k, v);

        return *this;
    }

    template <typename T>
    MatT<T> MatT<T>::transpose() const
    {
        MatT<T> t(h, w, c);

        for (int k = 0; k != t.c; ++k)
            for (int y = 0; y != t.h; ++y)
                for (int x = 0; x != t.w; ++x)
                    t.set(x, y, k, get(y, x, k));

        return t;
    }

    template <typename T>
    T MatT<T>::sum(int c)
    {
        T value = 0.0;

        for (int i = 0; i != w * h; ++i)
            value += data[c * h * w + i];

        return value;
    }

    template <typename T>
    T MatT<T>::max(int c)
    {
        T value = std::numeric_limits<T>::min();

        for (int i = 0; i != w * h; ++i)
        {
            T current = data[c * h * w + i];
            if (current > value)
            {
                value = current;
            }
        }

        return value;
    }

    template <typename T>
    T MatT<T>::min(int c)
    {
        T value = std::numeric_limits<T>::max();

        for (int i = 0; i != w * h; ++i)
        {
            T current = data[c * h * w + i];
            if (current < value)
            {
                value = current;
            }
        }

        return value;
    }

    template <typename T>
    void MatT<T>::minNmax(int c, T &minv, T &maxv)
    {
        minv = std::numeric_limits<T>::max();
        maxv = std::numeric_limits<T>::min();

        for (int i = 0; i != w * h; ++i)
        {
            T current = data[c * h * w + i];
            if (current < minv)
            {
                minv = current;
            }

            if (current > maxv)
            {
                maxv = current;
            }
        }
    }

    template <typename T>
    MatT<T> &MatT<T>::featureNormalize(int c)
    {
        T max_v;
        T min_v;
        minNmax(c, min_v, max_v);

        T delta = max_v - min_v;

        if (equivalent(delta, T(0.0)))
            return zero();

        for (int i = 0; i != w * h; ++i)
            data[c * h * w + i] = (data[c * h * w + i] - min_v) / delta;

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::featureNormalize()
    {
        for (int k = 0; k != c; ++k)
            featureNormalize(k);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::l1Normalize(int c)
    {
        T channel_sum = sum(c);
        T value = 1.0f / channel_sum;
        return mult(c, value);
    }

    template <typename T>
    MatT<T> &MatT<T>::l1Normalize()
    {
        for (int k = 0; k != c; ++k)
            l1Normalize(k);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::clamp(int c, T min, T max)
    {
        for (int i = 0; i != w * h; ++i)
            data[c * h * w + i] = clampTo(data[c * h * w + i], min, max);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::clamp(T min, T max)
    {
        for (int k = 0; k != c; ++k)
            clamp(k, min, max);

        return *this;
    }

    template <typename T>
    MatT<T> &MatT<T>::clamp()
    {
        return clamp(0, 1);
    }


    template<typename T>
    MatT<T> &MatT<T>::constrain(T value)
    {
        return clamp(-value, value);
    }


    template <typename T>
    int MatT<T>::size() const
    {
        return w * h * c;
    }

    template <typename T>
    int MatT<T>::channelSize() const
    {
        return w * h;
    }

    template <typename T>
    void MatT<T>::reshape(int w, int h, int c)
    {
        if (this->w == w && this->h == h && this->c == c)
        {
            return;
        }

        shared_data.reset();
        data = nullptr;
        this->w = 0;
        this->h = 0;
        this->c = 0;

        if (w > 0 && h > 0 && c > 0)
        {
            this->w = w;
            this->h = h;
            this->c = c;

            data = static_cast<T *>(calloc(size_t(size()), sizeof(T)));
            if (data)
            {
                shared_data = std::shared_ptr<T>(data, free);
            }
        }
    }

    //
    // Math Matrix Code
    // Rows,Cols
    //

    template <typename T>
    const T &MatT<T>::operator()(const int row, const int col) const
    {
        return data[row * w + col];
    }

    template <typename T>
    T &MatT<T>::operator()(const int row, const int col)
    {
        return data[row * w + col];
    }

    template <typename T>
    MatT<T> &MatT<T>::setIdentity()
    {
        zero();

        for (int i = 0; i < h && i < w; ++i)
            (*this)(i, i) = 1;

        return *this;
    }

    template <typename T>
    MatT<T> MatT<T>::augment() const
    {
        MatT<T> c(w * 2, h);
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                c(i, j) = (*this)(i, j);

        for (int j = 0; j < h; ++j)
            c(j, j + w) = 1;

        return c;
    }

    template <typename T>
    MatT<T> MatT<T>::invert() const
    {
        MatT<T> none;

        // Matrix not square
        if (h != w)
            return none;

        MatT<T> c = augment();

        for (int k = 0; k < c.h; ++k)
        {
            T p = 0;
            int index = -1;
            for (int i = k; i < c.h; ++i)
            {
                T val = T(fabs(c(i, k)));
                if (val > p)
                {
                    p = val;
                    index = i;
                }
            }

            // "Can't do it, sorry!
            if (index == -1)
                return none;

            T swap;
            for (int j = 0; j != c.w; ++j)
            {
                swap = c(index, j);
                c(index, j) = c(k, j);
                c(k, j) = swap;
            }

            T val = c(k, k);
            c(k, k) = 1;
            for (int j = k + 1; j < c.w; ++j)
                c(k, j) /= val;

            for (int i = k + 1; i < c.h; ++i)
            {
                T s = -c(i, k);
                c(i, k) = 0;
                for (int j = k + 1; j < c.w; ++j)
                    c(i, j) += s * c(k, j);
            }
        }

        for (int k = c.h - 1; k > 0; --k)
            for (int i = 0; i < k; ++i)
            {
                T s = -c(i, k);
                c(i, k) = 0;
                for (int j = k + 1; j < c.w; ++j)
                    c(i, j) += s * c(k, j);
            }

        MatT<T> inv(w, h);
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                inv(i, j) = c(i, j + w);

        return inv;
    }

    template <typename T>
    MatT<T> MatT<T>::makeIdentity(int rows, int cols)
    {
        MatT<T> mat(cols, rows);
        return mat.setIdentity();
    }

    template <typename T>
    MatT<T> MatT<T>::makeIdentity3x3()
    {
        MatT<T> H(3, 3);
        H(0, 0) = 1;
        H(1, 1) = 1;
        H(2, 2) = 1;
        return H;
    }

    template <typename T>
    MatT<T> MatT<T>::makeTranslation3x3(T dx, T dy)
    {
        MatT<T> H = makeIdentity3x3();
        H(0, 2) = dx;
        H(1, 2) = dy;
        return H;
    }

    template <typename T>
    void MatT<T>::mmult(const MatT<T> &a, const MatT<T> &b, MatT<T> &p)
    {
        assert(a.w == b.h);
        p.reshape(b.w, a.h, 1);
        p.zero();

        for (int i = 0; i < p.h; ++i)
            for (int j = 0; j < p.w; ++j)
                for (int k = 0; k < a.w; ++k)
                    p(i, j) += a(i, k) * b(k, j);
    }

    template <typename T>
    MatT<T> MatT<T>::mmult(const MatT<T> &a, const MatT<T> &b)
    {
        MatT<T> p;
        mmult(a, b, p);
        return p;
    }

    template <typename T>
    void MatT<T>::vmult(const MatT<T> &a, const MatT<T> &b, MatT<T> &p)
    {
        assert(b.size() == a.w);

        p.reshape(b.w, a.h, 1);
        p.zero();

        for (int i = 0; i < a.h; ++i)
            for (int j = 0; j < a.w; ++j)
                p.data[i] += a(i, j) * b.data[j];
    }

    template <typename T>
    MatT<T> MatT<T>::vmult(const MatT<T> &a, const MatT<T> &b)
    {
        MatT<T> p;
        vmult(a, b, p);
        return p;
    }

    template <typename T>
    MatT<T> MatT<T>::llsSolve(MatT<T> const &M, MatT<T> const &b)
    {

        MatT<T> none;
        MatT<T> Mt = M.transpose();
        MatT<T> MtM = MatT<T>::mmult(Mt, M);
        MatT<T> MtMinv = MtM.invert();

        if (MtMinv.size() == 0)
            return none;

        MatT<T> Mdag = MatT<T>::mmult(MtMinv, Mt);
        MatT<T> a = MatT<T>::mmult(Mdag, b);

        return a;
    }

    //
    // force template instantiation
    //
    template class MatT<float>;
    template void MatT<float>::convert(MatT<double> &out);
    template MatT<double> MatT<float>::convert();

    template class MatT<double>;
    template void MatT<double>::convert(MatT<float> &out);
    template MatT<float> MatT<double>::convert();

    template class MatT<long long>;
}
