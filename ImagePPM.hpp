// Read and write PPM images
#ifndef ImagePPM_hpp
#define ImagePPM_hpp

#include "Vec.hpp"

struct ImagePPM {
    typedef Vec3c color_type;

    unsigned int width, height; // image size
    color_type *image;          // image data in [y][x][color] order

// public methods
public:
    // create from file
    ImagePPM(const char *filename);

    // create blank image given size
    ImagePPM(unsigned int width, unsigned int height);

    // destroy when done
    ~ImagePPM() { delete[] image; }

    // access a pixel as ImagePPM(x,y)
    color_type operator()(unsigned int tx, unsigned int ty) const {
        return image[ty*width + tx];
    }
    color_type &operator()(unsigned int tx, unsigned int ty) {
        return image[ty*width + tx];
    }

    // write image as a PPM
    void write(const char *filename);
};

#endif
