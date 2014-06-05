// vertex shader for octahedral light marker
#version 400 core

// transformation matrices
layout(std140) uniform Matrices {
    mat4 modelViewMatrix, modelViewInverse;
    mat4 projectionMatrix, projectionInverse;
    vec3 lightpos;
};

// per-vertex input
in vec3 vPosition;

void main() {
    // light position in MVP
    vec4 lighPos = projectionMatrix * modelViewMatrix * vec4(lightpos, 1);

    vec3 lightpos_real;
    lightpos_real.x = lightpos.y;
    lightpos_real.y = lightpos.x;
    lightpos_real.z = lightpos.z;

    gl_Position = projectionMatrix * modelViewMatrix * vec4(8*vPosition + lightpos_real,1);
}
