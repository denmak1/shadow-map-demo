// handle view changes
#ifndef View_hpp
#define View_hpp

#include "MatPair.hpp"
#include "Mat.hpp"

struct GLFWwindow;

class View {
// private data
private:
    // GL uniform buffer IDs
    enum {MATRIX_BUFFER, LIGHT_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

// public data
public:
    int width, height;            // current window dimensions
    
    Vec3f viewSpherical;          // view position in spherical coordinates
    Vec3f lightSpherical;         // light position in spherical coordinates
    float lightSphFixedY;         // create lightView mat with this y-value

    Vec3f camPos;

    float blur;                   // post processing blur value

// public methods
public:
    struct ShaderData {
        MatPair4f viewmat, projection;  // viewing matrices
        Vec3f lightpos;                 // light position matrix
    } sdata;

    struct LightData {
        MatPair4f lightViewMat, lightProjMat;   // view from light matricies
        Mat4f biasMatrix;                       // bias matrix for normalizing
    } ldata;

    // create with initial window size and orbit location
    View(GLFWwindow *win);

    // set up new window viewport and projection
    void viewport(GLFWwindow *win);

    // set view using orbitAngle
    void view();

    // set new light position
    void light();

    // update view parameters in shader
    void update() const;

    // print spherical coords of light and camera
    void printSphCoords();

    // print cartesian coords of light and camera
    void printCartCoords();
};

#endif
