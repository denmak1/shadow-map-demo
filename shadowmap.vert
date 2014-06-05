// shadow map vertex shader
#version 400 core

// transformation matrices
layout(std140) uniform Matrices {
    mat4 modelViewMatrix, modelViewInverse;
    mat4 projectionMatrix, projectionInverse;
    vec3 lightpos;
};

// light matrices
layout(std140) uniform lightMats {
    mat4 lightViewMat, lightViewMatInv;
    mat4 lightProjMat, lightProjMatInv;
    mat4 biasMat;
};

// per-vertex input
in vec3 vPosition;
in vec2 vUV;

// output to frag shader
out vec2 texcoord;
out vec3 position;
out vec4 position4;

void main()
{
    texcoord = vUV;

    // enable this to draw shadow map from camera pov
    if(false) {
        vec4 pos = modelViewMatrix * vec4(vPosition, 1);
        position = pos.xyz / pos.w;

        position4 = pos;
        gl_Position = projectionMatrix * pos;
    }

    // enable this to draw shadow map from light pov
    if(true) {

        // convert frag coords to light view space
        vec4 pos = lightViewMat * vec4(vPosition, 1);
        position = pos.xyz / pos.w;
        
        position4 = pos;
        gl_Position = lightProjMat * pos;
    }
} 