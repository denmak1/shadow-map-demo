Shadow Mapping Demo


All development was done in Visual Studio 2012 and only tested on Windows.
I don't have a Linux machine capable of OpenGL 3+, so I was unable to test it
there, but it should be fine on Linux as well, with necessary makefile changes.


===============================
File notes:
GLdemo.cpp has the initialization and startup code, as well as the
main drawing loop. This file does the calls to the functions necesasry for
rednering each pass.

ImagePPM.cpp is simple ppm reader/writer

Input.cpp handles mouse motion and keyboard input. Both orbit the view
around the center of the scene.

Octahedron.cpp is the code for a simple octahedral light marker that orbits
the center of the scene.

Shader.cpp contains code for loading shaders written in GLSL (GL shader
language)

Terrain.cpp loads and draws the terrain geometry.

View.cpp handles window and view changes.

various ppm image files: these files are either textures, terrain maps
or depth/bump/gloss maps for the pebbles texture. Some of them are
unused in the program but are included anyway.

*.vert = vertex shaders
*.frag = corresponding fragment shaders


===============================
Development notes:
This program attempts to implement shadow maps for a scene with some simple
geometry. This is essentially done by rendering the scene from the light's
PoV and storing depth values of each fragment in a texture during the first
pass. Then the depth values are used in the second pass to compare each
fragment to see if it is in shadow in the fragment shader.

First pass -
Each fragment of the terrain is transformed into light space, The depths are
also stored here. First, the depth value clipping distances were set. This is to
make sure that the texture doesn't come out completely white due to the depths
ranging from 0.9999 - 1.0000. The fixed depths are the ones that are written
to the texture.

Second pass -
Each vertex of the terrain is transformed into a fragment in light space again.
This time, the visibility factor is determined by comparing the transformed
fragment with the fragment in the texture shadowMap. If the depth of the
transformed fragment is less than the depth in the shadowMap, then the fraagment
is considered to be in shadow.

Keys -
    1 - 8: switch between various terrains
    T:     toggle view mode between:
               shadow map from light's PoV
               scene with shadows
    I:     zoom camera in
    O:     zoom camera out
    R:     reload shaders


===============================
Known Issues:
Artifacting around the edges. This might be due to me using an RGB texture
instead of a depth component. Since RGB is 8 bit, it cannot hold as much
precision as float depth components per fragment.


===============================
Compiling and running the demo (on Windows with Visual Studio):
First, the system should be capable of OpenGL 4+

Then, the necessary libraries should be downloaded:
1. GLFW:
    Download Win32 binaries from http://www.glfw.org/download.html
    Extract them into a directory on your disk
    Add a new system variable called GLFW32DIR and point to that directory
    
2. GLEW:
    Download Win32 binaries from http://glew.sourceforge.net/
    Extract them into a directory on your disk
    Add a new system variable called GLEWDIR and point to that directory
    
Then open the solution in Visual Studio (tested only with 2012 version, may or
may not work with others), compile and run.


===============================
External resources used:
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
http://www.paulsprojects.net/tutorials/smt/smt.html
http://ogldev.atspace.co.uk/www/tutorial23/tutorial23.html
http://fabiensanglard.net/shadowmapping/index.phphttp://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/shadowmapping/Tutorial%2014%20-%20Shadow%20Mapping.pdf

