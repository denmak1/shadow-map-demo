// set up and maintain view as window sizes change
#include <stdio.h>
#include "View.hpp"
#include "MatPair.inl"
#include "AppContext.hpp"
#include "Vec.inl"

// using core modern OpenGL
#ifdef _WIN32
#  include <GL/glew.h>
#else
#  define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

// for offsetof
#include <cstddef>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

//
// create and initialize view
//
View::View(GLFWwindow *win) :
    viewSpherical(vec3<float>(0.f, -1.4f, 500.f)),
    lightSpherical(vec3<float>(0.5f * F_PI, 0.25f * F_PI, 900.f)),
    lightSphFixedY(0.25f * F_PI)
{
    // define bias matrix for shadow mao
    ldata.biasMatrix = mat4<float>(0.5f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.5f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.5f, 0.0f,
                                   0.5f, 0.5f, 0.5f, 1.0f);
    
    // create uniform buffer objects
    glGenBuffers(NUM_BUFFERS, bufferIDs);

    // buffer for camera matrices
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderData), 0, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, AppContext::MATRIX_UNIFORMS,
                     bufferIDs[MATRIX_BUFFER]);

    // buffer for light matrices
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[LIGHT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), 0, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, AppContext::LIGHT_UNIFORMS,
                     bufferIDs[LIGHT_BUFFER]);

    // update view
    viewport(win);
    view();
    light();
    blur = 0.0032f;
}

//
// New view, pointing to origin, at specified angle
//
void View::view()
{
    // update viewing matrices
    sdata.viewmat = translate4fp(vec3<float>(0,0,-viewSpherical.z)) 
        * xrotate4fp(viewSpherical.y)
        * zrotate4fp(viewSpherical.x);

    // update position from spherical coordinates
    float vcx = cos(viewSpherical.x), vsx = sin(viewSpherical.x);
    float vcy = cos(viewSpherical.y), vsy = sin(viewSpherical.y);
    camPos = viewSpherical.z * vec3(vcx*vcy, vsx*vcy, vsy);
}

//
// New light position, pointing to origin as well
//
void View::light()
{
    // build light view matrix for rendering depth map
    ldata.lightViewMat = translate4fp(vec3<float>(0, 0, -lightSpherical.z)) 
        * xrotate4fp(lightSphFixedY)
        * zrotate4fp(lightSpherical.x);

    // update position from spherical coordinates
    // x = theta, y = phi
    float cx = cos(lightSpherical.x), sx = sin(lightSpherical.x);
    float cy = cos(lightSpherical.y), sy = sin(lightSpherical.y);
    sdata.lightpos = lightSpherical.z * vec3(cx*cy, sx*cy, sy);


    //sdata.lightpos = lightSpherical.z * vec3(sx*cy, sx*sy, cx);

    // build light projection matrix, renders depth map into 1024x1024 texture
    ldata.lightProjMat = perspective4fp(float(F_PI/4), (float)512/512,
                                        1, 10000) * zrotate4fp(F_PI);

    // update uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 
                    offsetof(ShaderData, lightpos), sizeof(sdata.lightpos),
                    &sdata.lightpos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // bind bias matrix to light buffer
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[LIGHT_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 
                    offsetof(LightData, biasMatrix), sizeof(ldata.biasMatrix),
                    &ldata.biasMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//
// This is called when window is created or resized
// Adjust projection accordingly.
//
void View::viewport(GLFWwindow *win)
{
    // get window dimensions
    glfwGetFramebufferSize(win, &width, &height);

    // this viewport makes a 1 to 1 mapping of physical pixels to GL
    // "logical" pixels
    glViewport(0, 0, width, height);

    // adjust 3D projection into this window
    sdata.projection = perspective4fp(float(F_PI/4), (float)width/height,
                                      1, 10000);
}

//
// call before drawing each frame to update per-frame scene state
//
void View::update() const
{
    // update uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderData), &sdata);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[LIGHT_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &ldata);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//
// print spherical coords (debugging)
//
void View::printSphCoords()
{
    printf("viewSph pos:  (%f, %f, %f)\n",viewSpherical.x,
           viewSpherical.y, viewSpherical.z);

    printf("lightSph pos: (%f, %f, %f)\n",lightSpherical.x,
           lightSpherical.y, lightSpherical.z);

    printf("lightSph fix: (%f, %f, %f)\n",lightSpherical.x,
           lightSphFixedY, lightSpherical.z);
}

//
// print spherical coords (debugging)
//
void View::printCartCoords()
{
    //printf("light mats:  %d %d %d\n", view->sdata.lightViewMat,
    //       view->sdata.lightProjMat, view->sdata.biasMatrix);
    //printf("camera mats: %d %d\n", view->sdata.viewmat,
    //       view->sdata.projection);

    printf("camera pos:   (%f, %f, %f)\n", camPos.x,
           camPos.y, camPos.z);

    printf("light pos:    (%f, %f, %f)\n", sdata.lightpos.x,
           sdata.lightpos.y, sdata.lightpos.z);

}