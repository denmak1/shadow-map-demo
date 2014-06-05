// shadow map fragment shader
#version 400 core

// input from vertex shader
in vec2 texcoord;
in vec3 position;
in vec4 position4;

// output to frame buffer
out vec4 fragColor;
//out float fragdepth;

void main()
{
    // alternate normalization method
    if(false) {
        float near = gl_DepthRange.near;
        float far = gl_DepthRange.far;

        float ndcDepth = (2.0*gl_FragCoord.z - near-far) / (far-near);
        float clipDepth = ndcDepth / gl_FragCoord.w;

        fragColor = vec4((clipDepth * 0.5) + 0.5);
    }

    // normalize and output frag depth
    if(true) {
        // adjust near and far depth distances
        float zNear = 0.05;
        float zFar = 100.0;

        float fragdepth = (2.0 * zNear) /
            (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

        fragColor = vec4(fragdepth);
        //fragdepth = gl_FragCoord.z;
    }
} 