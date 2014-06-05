// NxN-dimensional matrix and inverse class
#ifndef MatPair_hpp
#define MatPair_hpp

#include "Mat.hpp"

//////////////////////////////////////////////////////////////////////
// pair of NxN matrices (matrix and inverse)
template <typename T, int N>
struct MatPair {
    typedef T value_type;

    Mat<T,N> matrix;
    Mat<T,N> inverse;
};

//////////////////////////////////////////////////////////////////////
// 2x2, 3x3 and 4x4 float specializations
typedef MatPair<float,2> MatPair2f;
typedef MatPair<float,3> MatPair3f;
typedef MatPair<float,4> MatPair4f;

#endif
