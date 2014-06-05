// simple function to read a PPM and return the size and date
// ppm because they're so easy to read and write without external libraries

// ppm files start with header lines, separated by white space (space,
// tab, return, etc). The header is "P6 <width> <height> <maxvalue>"
// after the header, there is one more white-space character, then
// raw uncompressed byte data in [height][width][rgb] order

// it would be cleaner to throw/catch errors, but they just print & exit

#include "ImagePPM.hpp"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
// don't complain if we use standard IO functions instead of windows-only
#pragma warning( disable: 4996 )
#endif

//
// create from file
//
ImagePPM::ImagePPM(const char *name)
{
    // open file
    FILE *fp = fopen(name,"rb");
    if (!fp) {
        fprintf(stderr, "error opening %s\n", name);
        exit(1);
    }

    // check that "magic number" at beginning of file is P6
    if (fgetc(fp) != 'P' || fgetc(fp) != '6') {
        fprintf(stderr, "unknown image format %s\n", name);
        exit(1);
    }

    // the following really needs some more error checking...
    // read image size, maximum value, and blank following header
    fscanf(fp, " #%*[^\n]");    // skip comment (if there)
    fscanf(fp, "%d %d", &width, &height); // read image size
    fscanf(fp, " #%*[^\n]");    // skip comment (if there)
    fscanf(fp, "%*d");          // skip max value
    fgetc(fp);                  // skip final \n before data
    
    // allocate image and read array
    image = new color_type[width * height];
    fread(image, sizeof(color_type), width * height, fp);

    // done!
    fclose(fp);
}

//
// create empty image given size
//
ImagePPM::ImagePPM(unsigned int w, unsigned int h)
    : width(w), height(h), image(new color_type[w*h])
{}

//
// write image as PPM
//
void ImagePPM::write(const char *name)
{
    // open file
    FILE *fp = fopen(name, "wb");
    if (!fp) {
        fprintf(stderr, "error creating %s\n", name);
        exit(1);
    }

    // write header then data
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    fwrite(image, sizeof(color_type), width * height, fp);

    // close file
    fclose(fp);
}
