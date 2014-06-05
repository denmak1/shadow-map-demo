// draw a simple terrain height field
#include <stdio.h>
#include <math.h>
#include "Terrain.hpp"
#include "AppContext.hpp"
#include "ImagePPM.hpp"
#include "Vec.inl"

// using core modern OpenGL
#ifdef _WIN32
#  include <GL/glew.h>
#else
#  define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

//
// load the terrain data
//
Terrain::Terrain(const char *elevationPPM, const char *texturePPM,
                 const char *normalPPM, const char *glossPPM,
                 const char *heightPPM, unsigned int fbow, unsigned int fboh)
{
    printf("%s", elevationPPM);

    // buffer objects to be used later
    glGenTextures(NUM_TEXTURES, textureIDs);
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(NUM_VARRAYS, varrayIDs);
    

    // load textures into texture buffer and framebuffer
    loadTexture(texturePPM, textureIDs[COLOR_TEXTURE]);
    loadTexture(normalPPM, textureIDs[BUMP_TEXTURE]);
    loadTexture(glossPPM, textureIDs[GLOSS_TEXTURE]);
    loadTexture(heightPPM, textureIDs[HEIGHT_TEXTURE]);


    ///////////////
    // make a texture for the post-processing framebuffer
    glGenFramebuffers(1, &fboID);
    glGenRenderbuffers(1, &rboID);

    glGenTextures(1, &fbtex);
    glBindTexture(GL_TEXTURE_2D, fbtex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 640, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create and bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbtex, 0);
    
    // create renderbuffer for depth test info
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          GL_TEXTURE_WIDTH, GL_TEXTURE_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach renderbuffer to depth attachment point
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rboID);

    // check fbo status
    checkFBOstatus();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    ///////////////
    // make a texture for the shadow map
    glGenFramebuffers(1, &smfboID);
    glGenRenderbuffers(1, &smrboID);

    glGenTextures(1, &smTexture);
    glBindTexture(GL_TEXTURE_2D, smTexture);

    // RGBA8 texture
    if (true) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }

    // DEPTH_COMPONENT texture
    if(false) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // create and bind shadow map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, smfboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, smTexture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, smrboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          GL_TEXTURE_WIDTH, GL_TEXTURE_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach renderbuffer to depth attachment point
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, smrboID);

    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

    // check fbo status
    checkFBOstatus();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    ///////////////
    // load terrain heights
    drawVertices(elevationPPM, 2);


    ///////////////
    // load quad vertices
    float quadPositions[] = {-1.0, -1.0,
                              1.0, -1.0,
                              1.0,  1.0,
                              1.0,  1.0,
                             -1.0,  1.0,
                             -1.0, -1.0 };

    float quadTexcoords[] = { 0.0,  0.0,
                              1.0,  0.0,
                              1.0,  1.0,
                              1.0,  1.0,
                              0.0,  1.0,
                              0.0,  0.0 };

    // generate a quad for the fbo
    glGenBuffers(QNUM_BUFFERS, quadBufferIDs);
    glGenVertexArrays(QNUM_VARRAYS, quadVarrayIDs);

    glBindBuffer(GL_ARRAY_BUFFER, quadBufferIDs[QPOSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), quadPositions,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, quadBufferIDs[QTEXCOORD_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), quadTexcoords,
                 GL_STATIC_DRAW);

    // load terrain shaders
    shaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    shaderParts[0].file = "terrain.vert";
    shaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    shaderParts[1].file = "terrain.frag";
    shaderID = glCreateProgram();

    // load post-processing shaders
    ppShaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    ppShaderParts[0].file = "terrainfb.vert";
    ppShaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    ppShaderParts[1].file = "terrainfb.frag";
    ppShaderID = glCreateProgram();

    // load shadow map shaders
    smShaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    smShaderParts[0].file = "shadowmap.vert";
    smShaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    smShaderParts[1].file = "shadowmap.frag";
    smShaderID = glCreateProgram();

    updateShaders();
}

//
// Delete terrain data
//
Terrain::~Terrain()
{
    glDeleteProgram(shaderID);
    glDeleteProgram(ppShaderID);
    glDeleteProgram(smShaderID);
    glDeleteTextures(NUM_TEXTURES, textureIDs);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
    glDeleteBuffers(QNUM_BUFFERS, quadBufferIDs);

    glDeleteFramebuffers(1, &smfboID);
    glDeleteFramebuffers(1, &fboID);
    glDeleteRenderbuffers(1, &rboID);

    delete[] indices;
    delete[] texcoord;
    delete[] norm;
    delete[] dPdv;
    delete[] dPdu;
    delete[] vert;
}

//
// load a texture
//
void Terrain::loadTexture(const char *ppm, unsigned int textureID)
{
    ImagePPM texture(ppm);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                 texture.width, texture.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture.image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//
// load (or replace) terrain shaders
//
void Terrain::updateShaders()
{
    ///////////////
    // render pass shader and attributes
    loadShaders(shaderID, sizeof(shaderParts)/sizeof(*shaderParts), 
                shaderParts);
    glUseProgram(shaderID);

    // (re)connect camera view and projection matrices
    glUniformBlockBinding(shaderID, 
                          glGetUniformBlockIndex(shaderID,"Matrices"),
                          AppContext::MATRIX_UNIFORMS);

    // (re)connect light view and projection matrices
    glUniformBlockBinding(shaderID, 
                          glGetUniformBlockIndex(shaderID,"lightMats"),
                          AppContext::LIGHT_UNIFORMS);

    // texture
    glUniform1i(glGetUniformLocation(shaderID, "colorTexture"), 1);

    // normal (bump) map
    glUniform1i(glGetUniformLocation(shaderID, "bumpMap"), 2);

    // gloss map
    glUniform1i(glGetUniformLocation(shaderID, "glossMap"), 3);

    // height map
    glUniform1i(glGetUniformLocation(shaderID, "heightMap"), 4);

    // re-connect attribute arrays
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);

    positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    normalAttrib = glGetAttribLocation(shaderID, "vNormal");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttrib);

    uvAttrib = glGetAttribLocation(shaderID, "vUV");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(uvAttrib);

    dPduAttrib = glGetAttribLocation(shaderID, "dPdu");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[DPDU_BUFFER]);
    glVertexAttribPointer(dPduAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(dPduAttrib);

    dPdvAttrib = glGetAttribLocation(shaderID, "dPdv");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[DPDV_BUFFER]);
    glVertexAttribPointer(dPdvAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(dPdvAttrib);

    vertAttrib = glGetAttribLocation(shaderID, "vertex");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VERTEX_BUFFER]);
    glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertAttrib);

    // turn off everything we enabled
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);


    ///////////////
    // post-processing shaders and attributes
    loadShaders(ppShaderID, sizeof(ppShaderParts)/sizeof(*ppShaderParts), 
                ppShaderParts);
    glUseProgram(ppShaderID);

    glBindVertexArray(quadVarrayIDs[QUAD_VARRAY]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferIDs[QPOSITION_BUFFER]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    quadTexcoordAttrib = glGetAttribLocation(ppShaderID, "vt");
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferIDs[QTEXCOORD_BUFFER]);
    glVertexAttribPointer(quadTexcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(quadTexcoordAttrib);

    // disable
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);


    ///////////////    
    // shadow map shaders and attributes
    loadShaders(smShaderID, sizeof(smShaderParts)/sizeof(*smShaderParts), 
                smShaderParts);
    glUseProgram(smShaderID);

    glUniformBlockBinding(smShaderID, 
                          glGetUniformBlockIndex(smShaderID,"Matrices"),
                          AppContext::MATRIX_UNIFORMS);

    glUniformBlockBinding(smShaderID, 
                          glGetUniformBlockIndex(smShaderID,"lightMats"),
                          AppContext::LIGHT_UNIFORMS);

    positionAttrib = glGetAttribLocation(smShaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    uvAttrib = glGetAttribLocation(smShaderID, "vUV");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(uvAttrib);

    // disable
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//
// draws the terrain from camera pov
//
void Terrain::draw() const
{
    // printf("rendering terrain to fbo texture\n");

    // enable shaders
    glUseProgram(shaderID);

    // enable vertex array
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smTexture);

    /*
    // enable textures
    for(int i=0; i<NUM_TEXTURES; ++i) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
    } */

    // draw the triangles for each three indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, 3*numtri, GL_UNSIGNED_INT, 0);

    /*
    // turn of whatever we turned on
    for(int i=0; i<NUM_TEXTURES; ++i) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    } */

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//
// post process render
//
void Terrain::postprocess(float blur) const
{
    // printf("rendering fbo texture to screen, blur = %f\n", blur);

    // enable shaders
    glUseProgram(ppShaderID);

    // enable vertex array for quad
    glBindVertexArray(quadVarrayIDs[QUAD_VARRAY]);

    // update blur range value
    glUniform1f(glGetUniformLocation(ppShaderID, "range"), blur);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbtex);
    glGenerateMipmap(GL_TEXTURE_2D);

    glDrawArrays(GL_TRIANGLES, 0, 6);       // draws full screen
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//
// prints frame buffer status
//
bool Terrain::checkFBOstatus() {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    printf("Status: %d, ", status);

    switch((GLenum) status) {
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("Framebuffer format not supported\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("Framebuffer missing attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf("Framebuffer incomplete attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            printf("Framebuffer missing draw buffer\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            printf("Framebuffer missing read buffer\n");
            return false;
        case GL_FRAMEBUFFER_COMPLETE:
            printf("Framebuffer complete\n");
            return true;
        default:
            printf("Framebuffer unknown error\n");
            return false;
    }

    return true;
}

//
// draw a shadow map depth texture from light pov
//
void Terrain::drawShadowMap(View *view)
{
    glUseProgram(smShaderID);

    //view->printCartCoords();
    view->printCartCoords();
    view->printSphCoords();
        
    // enable vertex array for terrain geometry
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);

    // draw the triangles for each three indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, 3*numtri, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//
// draws the vertices from a elevation texture and loads them into the GPU
//
void Terrain::drawVertices(const char *elevationPPM, int str)
{
    ImagePPM elevation(elevationPPM);
    unsigned int w = elevation.width, h = elevation.height;
    gridSize = vec3<float>(float(w), float(h), 255.f);
   
    // world dimensions
    mapSize = vec3<float>(512, 512, 50);

    // build vertex, normal and texture coordinate arrays
    // * x & y are the position in the terrain grid
    // * idx is the linear array index for each vertex
    numvert = (w + 1) * (h + 1);
    vert = new Vec3f[numvert];
    dPdu = new Vec3f[numvert];
    dPdv = new Vec3f[numvert];
    norm = new Vec3f[numvert];
    texcoord = new Vec2f[numvert];

    for(unsigned int y=0, idx=0;  y <= h;  ++y) {
        for(unsigned int x=0;  x <= w;  ++idx, ++x) {
            // 3d vertex location: x,y from grid location, z from terrain data
            vert[idx] = (vec3<float>(float(x), float(y), elevation(x%w, y%h).r*float(str))
                         / gridSize - 0.5f) * mapSize;

            // compute normal & tangents from partial derivatives:
            //   position =
            //     (u / gridSize.x - .5) * mapSize.x
            //     (v / gridSize.y - .5) * mapSize.y
            //     (elevation / gridSize.z - .5) * mapSize.z
            //   the u-tangent is the per-component partial derivative by u:
            //      mapSize.x / gridSize.x
            //      0
            //      d(elevation(u,v))/du * mapSize.z / gridSize.z
            //   the v-tangent is the partial derivative by v
            //      0
            //      mapSize.y / gridSize.y
            //      d(elevation(u,v))/du * mapSize.z / gridSize.z
            //   the normal is the cross product of these

            // first approximate du = d(elevation(u,v))/du (and dv)
            // be careful to wrap indices to 0 <= x < w and 0 <= y < h
            float du = (elevation((x+1)%w, y%h).r - elevation((x+w-1)%w, y%h).r)
                * 0.5f * mapSize.z / gridSize.z;
            float dv = (elevation(x%w, (y+1)%h).r - elevation(x%w, (y+h-1)%h).r)
                * 0.5f * mapSize.z / gridSize.z;

            // final tangents and normal using these
            dPdu[idx] = normalize(vec3<float>(mapSize.x/gridSize.x, 0, du));
            dPdv[idx] = normalize(vec3<float>(0, mapSize.y/gridSize.y, dv));
            norm[idx] = normalize(dPdu[idx] ^ dPdv[idx]);

            // 2D texture coordinate for rocks texture, from grid location
            texcoord[idx] = vec2<float>(float(x),float(y)) / gridSize.xy;
        }
    }

    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    numtri = 2*w*h;
    indices = new unsigned int[numtri][3];
    for(unsigned int y=0, idx=0; y<h; ++y) {
        for(unsigned int x=0; x<w; ++x, idx+=2) {
            indices[idx][0] = (w+1)* y    + x;
            indices[idx][1] = (w+1)* y    + x+1;
            indices[idx][2] = (w+1)*(y+1) + x+1;

            indices[idx+1][0] = (w+1)* y    + x;
            indices[idx+1][1] = (w+1)*(y+1) + x+1;
            indices[idx+1][2] = (w+1)*(y+1) + x;
        }
    }

    // load vertex and index array to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), vert, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), norm, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec2f), texcoord, 
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[DPDU_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), dPdu, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[DPDV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), dPdv, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VERTEX_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), vert, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 numtri*sizeof(unsigned int[3]), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::changeScene(int key)
{
    int x = key - 48;
    printf("changing scene to: %d\n", x);

    const char *maps[9];
    maps[0] = "scene0.ppm"; maps[1] = "scene1.ppm"; maps[2] = "scene2.ppm";
    maps[3] = "scene3.ppm"; maps[4] = "scene4.ppm"; maps[5] = "scene5.ppm";
    maps[6] = "scene6.ppm"; maps[7] = "scene7.ppm"; maps[8] = "scene8.ppm";

    drawVertices(maps[x], 2);
    updateShaders();
}