Project: Shadow Mapping
CMSC491
Dennis Makmak

All development was done in Visual Studio 2012 and only tested on Windows.
I don't have a Linux machine capable of OpenGL 3+, so I was unable to test it
there, but it should be fine on Linux as well.


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
    1 - 5: switch between various terrains
    T:     toggle view mode between:
               shadow map from light's PoV
               frame buffer just drawing the shadowMap
               scene with shadows
    R:     reload shaders



Known Issues:
Artifacting. This might be due to me using an RGB texture instead of a depth
component. Since RGB is 8 bit, it cannot hold as much precision as float
depth components per fragment.


External resources used:
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
http://www.paulsprojects.net/tutorials/smt/smt.html
http://ogldev.atspace.co.uk/www/tutorial23/tutorial23.html
http://fabiensanglard.net/shadowmapping/index.phphttp://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/shadowmapping/Tutorial%2014%20-%20Shadow%20Mapping.pdf

