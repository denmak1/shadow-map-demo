// NxN-dimensional matrix class template
// based on ideas from Nathan Reed
//    http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
#ifndef Mat_hpp
#define Mat_hpp

#include "Vec.hpp"

//////////////////////////////////////////////////////////////////////
// NxN matrix
template <typename T, int N>
struct Mat {
    typedef T value_type;

    T data[N][N];

    // array-like access must be public member functions
    // all other operations as template functions in Vec.inl
    Vec<T,N> operator[](int i) const { return *(Vec<T,N>*)data[i]; }
    Vec<T,N> &operator[](int i) { return *(Vec<T,N>*)data[i]; }
};

//////////////////////////////////////////////////////////////////////
// 2x2, 3x3 and 4x4 float specializations
typedef Mat<float,2> Mat2f;
typedef Mat<float,3> Mat3f;
typedef Mat<float,4> Mat4f;

#endif
