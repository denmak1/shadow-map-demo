// handle changes due to mouse motion, or keys
#ifndef Input_hpp
#define Input_hpp

class View;
class Terrain;
class Octahedron;
struct GLFWwindow;

class Input {
// private data
private:
    int button, oldButton;      // which mouse button was pressed?
    double oldX, oldY;          // location of mouse at last event

    double updateTime;          // time (in seconds) of last update
    float panRate, tiltRate;    // for key change, orbiting rate in radians/sec

    float blurRate;

    int shadowMode;

// public data
public:
    bool redraw;                // true if we need to redraw

// public methods
public:
    float zoomRate;

    // initialize
    Input() : button(-1), oldButton(-1), oldX(0), oldY(0), panRate(0),
              tiltRate(0), blurRate(0), shadowMode(0), redraw(true) {}

    // handle mouse press / release
    void mousePress(GLFWwindow *win, int button, int action);

    // handle mouse motion
    void mouseMove(GLFWwindow *win, View *view, double x, double y);

    // handle key press
    void keyPress(GLFWwindow *win, int key, Terrain *, Octahedron *);

    // handle key release
    void keyRelease(GLFWwindow *win, int key);

    // update view (if necessary) based on key input
    void keyUpdate(View *view);

    // return mode of display
    int isShadowMode();
};

#endif
