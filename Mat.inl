// NxN-dimensional matrix class template
// based on ideas from Nathan Reed
//    http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
#ifndef Mat_inl
#define Mat_inl

#include "Mat.hpp"
#include "Vec.inl"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

//////////////////////////////////////////////////////////////////////
// matrix size
template <typename T, int N>
inline int dimensions(const Mat<T,N> &m) {
    return N;
}

//////////////////////////////////////////////////////////////////////
// Build functions. Not done as constructors, so the matrix will be a
// POD (plain old data) type for pre-C++11. Among other things,
// guarantees the type will be exactly equivalent to an array in
// memory.

// build small sizes from list of vectors
template <typename T>
Mat<T,2> mat2(Vec<T,2> a0, Vec<T,2> a1) {
    Mat<T,2> result;
    result[0] = a0;
    result[1] = a1;
    return result;
}

template <typename T>
Mat<T,3> mat3(Vec<T,3> a0, Vec<T,3> a1, Vec<T,3> a2) {
    Mat<T,3> result;
    result[0] = a0;
    result[1] = a1;
    result[2] = a2;
    return result;
}

template <typename T>
Mat<T,4> mat4(Vec<T,4> a0, Vec<T,4> a1, Vec<T,4> a2, Vec<T,4> a3) {
    Mat<T,4> result;
    result[0] = a0;
    result[1] = a1;
    result[2] = a2;
    result[3] = a3;
    return result;
}

// build small sizes from list of numbers
template <typename T>
Mat<T,2> mat2(T a00, T a01,
              T a10, T a11) {
    return mat2<T>(vec2<T>(a00,a01), vec2<T>(a10,a11));
}

template <typename T>
Mat<T,3> mat3(T a00, T a01, T a02,
              T a10, T a11, T a12,
              T a20, T a21, T a22) {
    return mat3<T>(vec3<T>(a00, a01, a02),
                   vec3<T>(a10, a11, a12),
                   vec3<T>(a20, a21, a22));;
}

template <typename T>
Mat<T,4> mat4(T a00, T a01, T a02, T a03,
              T a10, T a11, T a12, T a13,
              T a20, T a21, T a22, T a23,
              T a30, T a31, T a32, T a33) {
    return mat4<T>(vec4<T>(a00,a01,a02,a03),
                   vec4<T>(a10,a11,a12,a13),
                   vec4<T>(a20,a21,a22,a23),
                   vec4<T>(a30,a31,a32,a33));
}

// build 4x4 perspective from field of view (in radians), 
// x/y aspect ratio, and near/far clipping planes
Mat4f perspective4f(float fov, float aspect, float near, float far) {
    float y = 1/tanf(fov/2), x = y/aspect;
    float z = (near+far)/(near-far), w = 2*near*far/(near-far);
    return mat4<float>(x, 0, 0, 0,
                       0, y, 0, 0,
                       0, 0, z, -1,
                       0, 0, w, 0);
}

// build x, y, or z axis rotation for angle (in radians)
Mat4f xrotate4f(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return mat4<float>(1, 0, 0, 0,
                       0, c, s, 0,
                       0,-s, c, 0,
                       0, 0, 0, 1);
}
Mat4f yrotate4f(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return mat4<float>(c, 0,-s, 0,
                       0, 1, 0, 0,
                       s, 0, c, 0,
                       0, 0, 0, 1);
}
Mat4f zrotate4f(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return mat4<float>( c, s, 0, 0,
                       -s, c, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1);
}

// build 4x4 scale matrix
Mat4f scale4f(Vec3f s) {
    return mat4<float>(s.x, 0, 0, 0,
                       0, s.y, 0, 0,
                       0, 0, s.z, 0,
                       0, 0, 0, 1);
}

// build 4x4 translation matrix
Mat4f translate4f(Vec3f t) {
    return mat4<float>(1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       t.x, t.y, t.z, 1);
}

//////////////////////////////////////////////////////////////////////
// matrix addition and subtraction
template <typename T, int N>
inline Mat<T,N> operator+(const Mat<T,N> &m1, const Mat<T,N> &m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m1[i][j] + m2[i][j];
    return result;
}

template <typename T, int N>
inline Mat<T,N> operator-(const Mat<T,N> &m1, const Mat<T,N> &m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m1[i][j] - m2[i][j];
    return result;
}

//////////////////////////////////////////////////////////////////////
// scalar multiplication and division
template <typename T, int N>
inline Mat<T,N> operator*(T s, const Mat<T,N> &m) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = s * m[i][j];
    return result;
}

template <typename T, int N>
inline Mat<T,N> operator*(const Mat<T,N> &m, T s) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[i][j] * s;
    return result;
}

template <typename T, int N>
inline Mat<T,N> operator/(const Mat<T,N> &m, T s) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[i][j] / s;
    return result;
}

//////////////////////////////////////////////////////////////////////
// matrix*matrix, matrix*vector, and vector*matrix
template <typename T, int N>
inline Mat<T,N> operator*(const Mat<T,N> &m1, const Mat<T,N> &m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i) {
        for(int j=0; j<N; ++j) {
            result[j][i] = m1[0][i] * m2[j][0];

            for(int k=1; k<N; ++k)
                result[j][i] += m1[k][i] * m2[j][k];
        }
    }
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator*(const Vec<T,N> &v, const Mat<T,N> &m) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i) {
        result[i] = v[i] * m[0][i];

        for(int j=1; j<N; ++j)
            result[i] += v[i] * m[j][i];
    }
    return result;
}

template <typename T, int N>
inline Vec<T,N> operator*(const Mat<T,N> &m, const Vec<T,N> &v) {
    Vec<T,N> result;
    for(int j=0; j<N; ++j) {
        result[j] = m[j][0] * v[j];

        for(int i=1; i<N; ++i)
            result[j] += m[j][i] * v[j];
    }
    return result;
}

//////////////////////////////////////////////////////////////////////
// transpose (this'd be the place to add other matrix math operations)
template <typename T, int N>
Mat<T,N> transpose(const Mat<T,N> &m) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[j][i];
    return result;
}

#endif
