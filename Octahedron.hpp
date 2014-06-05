// octahedron signaling light location
#ifndef Octahedron_hpp
#define Octahedron_hpp

#include "Vec.hpp"
#include "Shader.hpp"

// tetrahedron data and rendering methods
class Octahedron {
// private data
private:
    unsigned int numvert;       // total vertices
    Vec3f vert[6];              // per-vertex position
    
    unsigned int numtri;             // total triangles
    Vec<unsigned int, 3> indices[8]; // 3 vertex indices per triangle

    // GL vertex array object IDs
    enum {TERRAIN_VARRAY, NUM_VARRAYS};
    unsigned int varrayIDs[NUM_VARRAYS];

    // GL buffer object IDs
    enum {POSITION_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // GL shaders
    unsigned int shaderID;      // ID for shader program
    ShaderInfo shaderParts[2];  // vertex & fragment shader info

    // vertex attribute IDs
    unsigned int positionAttrib;

// public methods
public:
    // create tetrahedron data
    Octahedron();

    // clean up allocated memory
    ~Octahedron();

    // load/reload shaders
    void updateShaders();

    // draw this tetrahedron object
    void draw() const;
};

#endif
