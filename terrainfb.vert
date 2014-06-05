// 2nd pass post processing vertex shader
#version 400 core

// per-vertex input
in vec2 vp;
in vec2 vt;

// output to fragment shader
out vec2 texcoord;

void main() {
    texcoord = vt;
    gl_Position = vec4(vp, 0.0, 1.0);
}