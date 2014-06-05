// terrain data and drawing
#ifndef Terrain_hpp
#define Terrain_hpp

#include "Vec.hpp"
#include "Mat.hpp"
#include "Shader.hpp"
#include "View.hpp"

// terrain data and rendering methods
class Terrain {
// private data
private:
    Vec3f gridSize;             // elevation grid size
    Vec3f mapSize;              // size of terrain in world space

    unsigned int numvert;       // total vertices
    Vec3f *vert;                // per-vertex position
    Vec3f *dPdu, *dPdv;         // per-vertex tangents
    Vec3f *norm;                // per-vertex normal
    Vec2f *texcoord;            // per-vertex texture coordinate

    unsigned int numtri;        // total triangles
    unsigned int (*indices)[3]; // 3 vertex indices per triangle

    // GL vertex array object IDs
    enum {TERRAIN_VARRAY, NUM_VARRAYS};
    unsigned int varrayIDs[NUM_VARRAYS];

    // GL texture IDs
    enum {COLOR_TEXTURE, BUMP_TEXTURE, GLOSS_TEXTURE, HEIGHT_TEXTURE,
          NUM_TEXTURES};
    unsigned int textureIDs[NUM_TEXTURES];

    // GL normal map IDs
    enum {NORMAL_ARRAY, NUM_NORMALS};
    unsigned int normalIDs[NUM_NORMALS];

    // GL buffer object IDs
    enum {POSITION_BUFFER, NORMAL_BUFFER, UV_BUFFER, DPDU_BUFFER, DPDV_BUFFER,
          VERTEX_BUFFER, INDEX_BUFFER, SM_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // terrain shaders
    unsigned int shaderID;      // ID for shader program
    ShaderInfo shaderParts[2];  // vertex & fragment shader info
    
    // vertex attribute IDs
    unsigned int positionAttrib, normalAttrib, uvAttrib, dPduAttrib,
                 dPdvAttrib, vertAttrib;
    

    // post-processing shaders
    unsigned int ppShaderID;
    ShaderInfo ppShaderParts[2];

    enum {QUAD_VARRAY, QNUM_VARRAYS};
    unsigned int quadVarrayIDs[QNUM_VARRAYS];

    enum {QPOSITION_BUFFER, QTEXCOORD_BUFFER, QNUM_BUFFERS};
    unsigned int quadBufferIDs[QNUM_BUFFERS];

    unsigned int quadPosAttrib, quadTexcoordAttrib;

    unsigned int rboID;
    unsigned int fbtex;


    // shadow map shaders
    unsigned int smShaderID;
    ShaderInfo smShaderParts[2];

    unsigned int smTexture;
    unsigned int smrboID;

// public methods
public:
    unsigned int fboID;
    unsigned int smfboID;

    // load terrain, given elevation image and surface texture
    Terrain(const char *elevationPPM, const char *texturePPM,
            const char *normalPPM, const char *glossPPM,
            const char *heightPPM, unsigned int fbow, unsigned int fboh);

    // clean up allocated memory
    ~Terrain();
    
    // load a texture
    void loadTexture(const char *ppm, unsigned int textureID);

    // load/reload shaders
    void updateShaders();

    // draw this terrain object
    void draw() const;

    // 2nd pass rendering on the terrain object
    void postprocess(float blur) const;

    // check frame buffer creation status
    bool checkFBOstatus();
   
    // draw shadow map from light pov
    void drawShadowMap(View *view);

    // draw vertices from elevation texture
    void drawVertices(const char *elevationPPM, int str);

    // change scene
    void changeScene(int key);
};

#endif
