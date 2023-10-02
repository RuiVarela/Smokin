#pragma once

#include <cmath>

namespace smk
{
    template<typename T> constexpr inline T epsilon() { return T(1e-6); }

    template<typename T> constexpr inline T minimum(T const left, T const right) { return ((left < right) ? left: right); }
    template<typename T> constexpr inline T minimum(T const a, T const b, T const c) { return minimum(minimum(a, b), c); }
    template<typename T> constexpr inline T minimum(T const a, T const b, T const c, T const d) { return minimum(minimum(minimum(a, b), c), d); }

    template<typename T> constexpr inline T maximum(T const left, T const right) { return ((left > right) ? left : right); }
    template<typename T> constexpr inline T maximum(T const a, T const b, T const c) { return maximum(maximum(a, b), c); }
    template<typename T> constexpr inline T maximum(T const a, T const b, T const c, T const d) { return maximum(maximum(maximum(a, b), c), d); }

    template<typename T> constexpr inline T absolute(T const value) { return ((value < T(0.0)) ? -value : value); }

    template<typename T> constexpr inline T round(T const value) { return ( (value >= T(0.0)) ? floor(value + T(0.5)) : ceil(value - T(0.5)) ); }

    template<typename T> constexpr inline T sign(T const value) { return ((value < T(0.0)) ? T(-1.0) : ((value == T(0.0)) ? T(0.0) : T(1.0)) ); }
    template<typename T> constexpr inline T signedSquare(T const value) { return ((value < T(0.0)) ? -value * value : value * value); }
    template<typename T> constexpr inline T square(T const value) { return value * value; }

    template<typename T> constexpr inline bool equivalent(T const l, T const r, T const epsilon = T(1e-6)) { T d = r - l; return (d < T(0.0)) ? (d >= -epsilon) : (d <= epsilon); }

    template<typename T> constexpr inline T clampNear(T const value, T const target, T const epsilon = 1e-6) { return (equivalent(value, target, epsilon) ? target : value); }
    template<typename T> constexpr inline T clampTo(T const value, T const minimum, T const maximum) { return ((value < minimum) ? minimum : ((value > maximum) ? maximum : value) ); }
    template<typename T> constexpr inline T clampAbove(T const value, T const minimum) { return ((value < minimum) ? minimum : value); }
    template<typename T> constexpr inline T clampBelow(T const value, T const maximum) { return ((value > maximum) ? maximum : value); }


    template<typename T> constexpr inline T sinc(T const& value) {
        if (absolute(value) < epsilon)
            return T(1.0) + value * value * ( T(-1.0) / T(6.0) + value * value * T(1.0) / T(120.0));
        else
            return sin(value) / value;
    }

    template<typename T> constexpr inline T fract(T const value) { return value - floor(value); }

    template<typename T> constexpr inline T smoothstep(T const edge0, T const edge1, T x) {
        // Scale, bias and saturate x to 0..1 range
        x = clampTo((x - edge0) / (edge1 - edge0), T(0.0), T(1.0));
        // Evaluate polynomial
        return x * x * (3 - 2 * x);
    }

    //
    // Repeats a space that is not bounded on the positive axis, i.e: that cannot be repeated with fmod
    // halfRange should be positive and it represents a range from [-halfRange, halfRange]
    //
    template<typename T> constexpr inline T repeatSpace(T const& half_range, T const& value) {
        const T start = -half_range;
        const T total = half_range - start;
        const T offset = (value - start) / total;

        T repeated_offset = std::fmod(offset, T(1.0));
        if (repeated_offset < 0.0f)
            repeated_offset += 1.0;

        return -half_range + repeated_offset * total;
    }

    //
    // angles
    //
    template<typename T> constexpr inline T piAngle() { return T(3.14159265358979323846); }
    template<typename T> constexpr inline T piDiv2() { return piAngle<T>() / T(2.0); }
    template<typename T> constexpr inline T piDiv4() { return piAngle<T>() / T(4.0); }
    template<typename T> constexpr inline T piDiv180() { return piAngle<T>() / T(180.0); }
    template<typename T> constexpr inline T piDiv180Inverse() { return T(180.0) / piAngle<T>(); }
    template<typename T> constexpr inline T twoPi() { return piAngle<T>() * T(2.0); }

    template<typename T> constexpr inline T radiansToDegrees(T const radians) { return radians * piDiv180Inverse<T>(); }
    template<typename T> constexpr inline T degreesToRadians(T const degrees) { return degrees * piDiv180<T>(); }
    template<typename T> constexpr inline T normalizeAngle360(T degrees) {
        while (degrees > T(360.0)) degrees -= T(360.0);
        while (degrees < T(0.0f))  degrees += T(360.0);
        return degrees;
    }
    template<typename T> constexpr inline T normalizeAngle180(T degrees) {
        degrees = normalizeAngle360(degrees);
        if (degrees > T(180.0))
          degrees -= T(180.0);
        return degrees;
    }


    //
    // random number between 0.0 and 1.0
    //
    float uniformRandom();
    float uniformRandomRange(float minv, float maxv);

}
