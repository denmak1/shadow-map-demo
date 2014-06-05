// set up and maintain view as window sizes change
#include <stdio.h>
#include "Input.hpp"
#include "View.hpp"
#include "Terrain.hpp"
#include "Octahedron.hpp"

// using core modern OpenGL
#ifdef _WIN32
#  include <GL/glew.h>
#else
#  define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

//
// called when a mouse button is pressed. 
// Remember where we were, and what mouse button it was.
//
void Input::mousePress(GLFWwindow *win, int b, int action)
{
    if (action == GLFW_PRESS) {
        // hide cursor, record button
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        button = b;
    }
    else {
        // display cursor, update button state
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        button = -1;       // no button
    }
}

//
// called when the mouse moves
// use difference between oldX,oldY and x,y to define a rotation
//
void Input::mouseMove(GLFWwindow *win, View *view, double x, double y)
{
    // only update view after at least one old position is stored
    if (button == GLFW_MOUSE_BUTTON_LEFT && button == oldButton) {
        // record differences & update last position
        float dx = float(x - oldX);
        float dy = float(y - oldY);

        // rotation angle, scaled so across the window = one rotation
        view->viewSpherical.x += float(M_PI) * dx / float(view->width);
        view->viewSpherical.y += 0.5f*float(M_PI) * dy / float(view->height);
        view->view();

        // tell GLFW that something has changed and we must redraw
        redraw = true;
    }

    // update prior mouse state
    oldButton = button;
    oldX = x;
    oldY = y;
}

//
// called when any key is pressed
//
void Input::keyPress(GLFWwindow *win, int key, Terrain *terrain,
                     Octahedron *lmarker)
{
    switch (key) {
    case 'A':                   // rotate left
        panRate = float(-M_PI); // half a rotation/sec
        updateTime = glfwGetTime();
        redraw = true;          // need to redraw
        break;
    case 'D':                   // rotate right
        panRate = float(M_PI);  // half a rotation/sec
        updateTime = glfwGetTime();
        redraw = true;          // need to redraw
        break;
    case 'W':                   // rotate up
        tiltRate = float(M_PI/2); // 1/4 rotation/sec
        updateTime = glfwGetTime();
        redraw = true;          // need to redraw
        break;
    case 'S':                   // rotate down
        tiltRate = float(-M_PI/2); // 1/4 rotation/sec
        updateTime = glfwGetTime();
        redraw = true;          // need to redraw
        break;
    case 'U':                   // increase blur range
        blurRate = 0.00005f;
        updateTime = glfwGetTime();
        redraw = true;
        break;
    case 'J':                   // decrease blur range
        blurRate = -0.00005f;
        updateTime = glfwGetTime();
        redraw = true;
        break;
    case 'O':                   // zoom out
        zoomRate = 10.0f;
        updateTime = glfwGetTime();
        redraw = true;
        break;
    case 'I':
        zoomRate = -10.0f;      // zoom in
        updateTime = glfwGetTime();
        redraw = true;
        break;
    case 'T':                   // toggle shadow map display mode
        shadowMode = (shadowMode + 1) % 2;
        printf("toggling shadow map display mode - %d\n", shadowMode);
        redraw = true;
        break;
    case 'R':                   // reload shaders
        lmarker->updateShaders();
        terrain->updateShaders();
        redraw = true;          // need to redraw
        break;
    case '0': case '1': case '2':
    case '3': case '4': case '5':
    case '6': case '7': case '8':
        terrain->changeScene(key);
        redraw = true;
        break;
    case GLFW_KEY_ESCAPE:       // Escape: exit
        glfwSetWindowShouldClose(win, true);
        break;
    }

}

//
// called when any key is released
//
void Input::keyRelease(GLFWwindow *win, int key)
{
    switch (key) {
    case 'A': case 'D':         // stop panning
        panRate = 0;
        break;
    case 'W': case 'S':         // stop tilting
        tiltRate = 0;
        break;
    case 'U': case 'J':         // stop blurring
        blurRate = 0;
        break;
    case 'I': case 'O':         // stop blurring
        zoomRate = 0;
        break;
    }
}

//
// update view if necessary based on a/d keys
//
void Input::keyUpdate(View *view)
{
    if (panRate != 0 || tiltRate != 0) {
        double now = glfwGetTime();
        double dt = (now - updateTime);

        view->lightSpherical.x += float(panRate * dt);
        view->lightSpherical.y += float(tiltRate * dt);

        // fix position in view mat
        view->lightSphFixedY += float(-tiltRate * dt);

        view->light();

        // remember time for next update
        updateTime = now;

        // changing, so will need to start another draw
        redraw = true;
    }

    else if (blurRate != 0.0f) {
        double now = glfwGetTime();
        view->blur += blurRate;

        if(view->blur <= 0.0f)
            view->blur = 0.0f;
        else if(view->blur >= 1.0f)
            view->blur = 1.0f;

        updateTime = now;
        redraw = true;
    }

    else if(zoomRate != 0.0f) {
        double now = glfwGetTime();
        view->viewSpherical.z += zoomRate;

        if(view->viewSpherical.z <= 100.0f)
            view->viewSpherical.z = 100.0f;
        else if(view->viewSpherical.z >= 2000.0f)
            view->viewSpherical.z = 2000.0f;

        // update
        updateTime = now;
        view->view();

        redraw = true;
    }
}

int Input::isShadowMode()
{
    return shadowMode;
}