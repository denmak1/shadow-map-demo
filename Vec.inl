// N-dimensional vector class template inline functions
// based on ideas from Nathan Reed
//    http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/

// note that since the size is a template parameter, it's a compile-time
// constant. Loops with N iterations should be unrolled by the compiler

#ifndef Vec_inl
#define Vec_inl

#include "Vec.hpp"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// vector size
template <typename T, int N>
inline int dimensions(const Vec<T,N> &v) {
    return N;
}

//////////////////////////////////////////////////////////////////////
// Build functions. Not done as constructors, so the vector will be a
// POD (plain old data) type for pre-C++11. Among other things,
// guarantees the type will be exactly equivalent to an array in
// memory.

// all components equal to single scalar value
template <typename T, int N>
Vec<T,N> vec(T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s;
    return result;
}

// build 2D, 3D or 4D from a list of components
template <typename T>
inline Vec<T,2> vec2(T x, T y) {
    Vec<T,2> result;
    result[0] = x;
    result[1] = y;
    return result;
}
template <typename T>
inline Vec<T,3> vec3(T x, T y, T z) {
    Vec<T,3> result;
    result[0] = x;
    result[1] = y;
    result[2] = z;
    return result;
}
template <typename T>
inline Vec<T,4> vec4(T x, T y, T z, T w) {
    Vec<T,4> result;
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
    return result;
}

//////////////////////////////////////////////////////////////////////
// component-wise operations: +, -, *, /, %
template <typename T, int N>
inline Vec<T,N> operator+(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i] + v2[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator-(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i] - v2[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator*(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i] * v2[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator/(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i] / v2[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator%(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i] % v2[i];
    return result;
}

//////////////////////////////////////////////////
// operations between a vector and a scalar: +, -, *, /, %
template <typename T, int N>
inline Vec<T,N> operator+(const Vec<T,N> &v, T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] + s;
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator-(const Vec<T,N> &v, T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] - s;
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator*(const Vec<T,N> &v, T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] * s;
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator/(const Vec<T,N> &v, T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] / s;
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator%(const Vec<T,N> &v, T s) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] % s;
    return result;
}

//////////////////////////////////////////////////
// operations between a scalar and vector: +, -, *, /, %
template <typename T, int N>
inline Vec<T,N> operator+(T s, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s + v[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator-(T s, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s - v[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator*(T s, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s * v[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator/(T s, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s / v[i];
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator%(T s, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = s % v[i];
    return result;
}


//////////////////////////////////////////////////////////////////////
// other linear algebra operations

// vector dot product, dot(v1, v2)
template <typename T, int N>
inline T dot(const Vec<T,N> &v1, const Vec<T,N> &v2) {
    T result = 0;
    for(int i=0; i<N; ++i)
        result += v1[i] * v2[i];
    return result;
}

// vector length, length(v)
template <typename T, int N>
inline T length(const Vec<T, N> &v) {
    return T(sqrt(dot(v, v)));
}

// normalized vector, normalize(v)
template <typename T, int N>
inline Vec<T,N> normalize(const Vec<T,N> &v) {
    return v / length(v);
}

// clamp between two values
template <typename T, int N>
inline Vec<T,N> clamp(const Vec<T,N> &v, T lo, T hi) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v[i] < lo ? lo : v[i] > hi ? hi : v[i];
    return result;
}

// linear interpolation
template <typename T, int N>
inline Vec<T,N> lerp(const Vec<T,N> &v1, const Vec<T,N> v2, float t) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i)
        result[i] = v1[i]*(1-t) + v2[i]*t;
    return result;
}

// cross product, v1^v2 for Vec<3> only
template <typename T>
inline Vec<T,3> operator^(const Vec<T,3> &v1, const Vec<T,3> &v2) {
    Vec<T,3> result;
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
    return result;
}

#endif
