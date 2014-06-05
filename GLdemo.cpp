//
// Simple GL example
//
#include "AppContext.hpp"
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

#include <stdio.h>

///////
// Clean up any context data
AppContext::~AppContext()
{
    // if any are NULL, deleting a NULL pointer is OK
    delete view;
    delete input;
    delete terrain;
    delete lmarker;
}

///////
// GLFW callbacks must use extern "C"
extern "C" {

    //
    // called for GLFW error
    //
    void winError(int error, const char *description)
    {
        fprintf(stderr, "GLFW error: %s\n", description);
    }

    //
    // called whenever the window size changes
    //
    void reshape(GLFWwindow *win, int width, int height)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->view->viewport(win);
        appctx->input->redraw = true;
    }

    //
    // called when mouse button is pressed
    //
    void mousePress(GLFWwindow *win, int button, int action, int mods)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->input->mousePress(win, button, action);
    }

    //
    // called when mouse is moved
    //
    void mouseMove(GLFWwindow *win, double x, double y)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->input->mouseMove(win, appctx->view, x,y);
    }

    // 
    // called on any keypress
    //
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        if (action == GLFW_PRESS)
            appctx->input->keyPress(win, key, appctx->terrain, appctx->lmarker);
        else if (action == GLFW_RELEASE)
            appctx->input->keyRelease(win, key);
    }
}

// initialize GLFW - windows and interaction
GLFWwindow *initGLFW(AppContext *appctx)
{
    // set error callback before init
    glfwSetErrorCallback(winError);
    if (! glfwInit())
        return 0;

    // using core OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow *win = glfwCreateWindow(1024, 640, "Project: Shadow Mapping",
                                       0, 0);
    if (! win) {
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(win);

#ifdef _WIN32
	// use GLEW on windows to access modern OpenGL functions
	glewExperimental = true;
	glewInit();
#endif

	// store context pointer to access application data
    glfwSetWindowUserPointer(win, appctx);

    // set callback functions to be called by GLFW
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);
    
    // set OpenGL state
    glEnable(GL_DEPTH_TEST);      // tell OpenGL to handle overlapping surfaces

    return win;
}

int main(int argc, char *argv[])
{
    // collected data about application for use in callbacks
    AppContext appctx;

    // set up GLUT and OpenGL
    GLFWwindow *win = initGLFW(&appctx);
    if (! win) return 1;

    // initialize context (after GLFW)
    appctx.view = new View(win);
    appctx.input = new Input;
    appctx.input->zoomRate = 0.0f;
    appctx.terrain = new Terrain("scene0.ppm", "pebbles.ppm",
        "pebbles-norm.ppm", "pebbles-gloss.ppm", "pebbles-bump.ppm",
        appctx.view->width, appctx.view->height);
    appctx.lmarker = new Octahedron();

    // loop until GLFW says it's time to quit
    while (!glfwWindowShouldClose(win)) {
        // check for continuous key updates to view
        appctx.input->keyUpdate(appctx.view);

        if (appctx.input->redraw) {
            // we're handing the redraw now
            appctx.input->redraw = false;
            appctx.view->update();

            // draw shadow map to screen only
            if(appctx.input->isShadowMode() == 1) {

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // set to resolution of shadow map tex
                glViewport(0, 0, 1024, 1024);

                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                appctx.terrain->drawShadowMap(appctx.view);
                //appctx.lmarker->draw();
            }

            // draw shadowmap fbo in a quad
            // debugging use only
            /*else if(appctx.input->isShadowMode() == 2) {
                // draw shadow map texture
                glBindFramebuffer(GL_FRAMEBUFFER, appctx.terrain->fboID);
                glViewport(0, 0, 1024, 1024);

                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                appctx.terrain->drawShadowMap(appctx.view);

                // just draw the texture on the screen
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                appctx.view->viewport(win);

                glClearColor(1.f, .9f, .8f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                appctx.terrain->postprocess(appctx.view->blur);
            }*/

            // draw scene with shadow map
            else {
                // render to the shadow map from light pov
                glBindFramebuffer(GL_FRAMEBUFFER, appctx.terrain->smfboID);
                glViewport(0, 0, 1024, 1024);

                glClearColor(0.5f, 0.2f, 0.7f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_FRONT);

                appctx.terrain->drawShadowMap(appctx.view);
           
                // render from camera pov with shadow map
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                appctx.view->viewport(win);

                glClearColor(.5f, .7f, .9f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_BACK);

                appctx.terrain->draw();
                appctx.lmarker->draw();

                /*
                // do something else in sceen space render pass????
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                appctx.view->viewport(win);

                glClearColor(1.f, .9f, .8f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                appctx.terrain->postprocess(appctx.view->blur); */
                
            }
            glfwSwapBuffers(win);
        }

        // wait for user input
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    
    return 0;
}
