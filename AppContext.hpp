// collected state for access in callbacks
#ifndef AppContext_hpp
#define AppContext_hpp

struct AppContext {
    class View *view;           // viewing data
    class Input *input;         // user interface data
    class Terrain *terrain;     // terrain geometry
    class Octahedron *lmarker;     // terrain geometry

    // uniform matrix block indices
    enum { MATRIX_UNIFORMS, LIGHT_UNIFORMS };

    // initialize all pointers to NULL to allow delete in destructor
    AppContext() : view(0), input(0), terrain(0), lmarker(0) {}

    // clean up any context data
    ~AppContext();
};

// load set of shaders
#endif
