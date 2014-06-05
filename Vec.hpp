// N-dimensional vector class template
// based on ideas from Nathan Reed
//    http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
#ifndef Vec_hpp
#define Vec_hpp

#include <math.h>

//////////////////////////////////////////////////////////////////////
// N-dimensional vector of any component type
template <typename T, int N>
struct Vec {
    typedef T value_type;

    T data[N];

    // array-like access must be public member functions
    // all other operations as template functions in Vec.inl
    T operator[](int i) const { return data[i]; }
    T &operator[](int i) { return data[i]; }
};

//////////////////////////////////////////////////////////////////////
// 2D, 3D and 4D specialization with component access
template <typename T>
struct Vec<T,2> {
    typedef T value_type;

    union {
        T data[2];
        struct { T x, y; };
    };
    T operator[](int i) const { return data[i]; }
    T &operator[](int i) { return data[i]; }
};

template <typename T>
struct Vec<T,3> {
    typedef T value_type;

    union {
        T data[3];
        struct { T x, y, z; };
        struct { T r, g, b; };
        Vec<T,2> xy;
    };
    T operator[](int i) const { return data[i]; }
    T &operator[](int i) { return data[i]; }
};

template <typename T>
struct Vec<T,4> {
    typedef T value_type;

    union {
        T data[4];
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        Vec<T,2> xy;
        Vec<T,3> xyz;
        Vec<T,3> rgb;
    };
    T operator[](int i) const { return data[i]; }
    T &operator[](int i) { return data[i]; }
};

//////////////////////////////////////////////////////////////////////
// 2D-4D float and color types
typedef Vec<float,2> Vec2f;
typedef Vec<float,3> Vec3f;
typedef Vec<float,4> Vec4f;

typedef Vec<unsigned char,2> Vec2c;
typedef Vec<unsigned char,3> Vec3c;
typedef Vec<unsigned char,4> Vec4c;

#endif
