// draw a simple tetrahedral light indicator
#include "Octahedron.hpp"
#include "AppContext.hpp"
#include "Vec.inl"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>


//
// load the geometry data
//
Octahedron::Octahedron()
{
    // buffer objects to be used later
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(NUM_VARRAYS, varrayIDs);

    // build vertex array
    numvert = sizeof(vert)/sizeof(*vert);
    vert[0] = vec3<float>( 1.f, 0.f, 0.f);
    vert[1] = vec3<float>(-1.f, 0.f, 0.f);
    vert[2] = vec3<float>( 0.f, 1.f, 0.f);
    vert[3] = vec3<float>( 0.f,-1.f, 0.f);
    vert[4] = vec3<float>( 0.f, 0.f, 1.f);
    vert[5] = vec3<float>( 0.f, 0.f,-1.f);

    // build index array linking sets of three vertices into triangles
    numtri = sizeof(indices)/sizeof(*indices);
    indices[0] = vec3<unsigned int>(0, 2, 4);
    indices[1] = vec3<unsigned int>(0, 4, 3);
    indices[2] = vec3<unsigned int>(0, 3, 5);
    indices[3] = vec3<unsigned int>(0, 5, 2);
    indices[4] = vec3<unsigned int>(1, 4, 2);
    indices[5] = vec3<unsigned int>(1, 2, 5);
    indices[6] = vec3<unsigned int>(1, 5, 3);
    indices[7] = vec3<unsigned int>(1, 3, 4);

    // load vertex and index array to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), vert, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 numtri*sizeof(unsigned int[3]), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // initial shader load
    shaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    shaderParts[0].file = "marker.vert";
    shaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    shaderParts[1].file = "marker.frag";
    shaderID = glCreateProgram();
    updateShaders();
}

//
// Delete lightmarker data
//
Octahedron::~Octahedron()
{
    glDeleteProgram(shaderID);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
}

//
// load (or replace) lightmarker shaders
//
void Octahedron::updateShaders()
{
    loadShaders(shaderID, sizeof(shaderParts)/sizeof(*shaderParts), 
                shaderParts);
    glUseProgram(shaderID);

    // (re)connect view and projection matrices
    glUniformBlockBinding(shaderID, 
                          glGetUniformBlockIndex(shaderID,"Matrices"),
                          AppContext::MATRIX_UNIFORMS);

    // re-connect attribute arrays
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);

    positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    // turn off everything we enabled
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//
// this is called every time the lightmarker needs to be redrawn 
//
void Octahedron::draw() const
{
    // enable shaders
    glUseProgram(shaderID);

    // enable vertex arrays
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);

    // draw the triangles for each three indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, 3*numtri, GL_UNSIGNED_INT, 0);

    // turn of whatever we turned on
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

