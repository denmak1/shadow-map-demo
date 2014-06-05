// vertex shader for simple terrain demo
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
in vec3 dPdu;
in vec3 dPdv;
in vec3 vNormal;
in vec2 vUV;
in vec3 vert;

// output to fragment shader
out vec3 position;
out vec3 light;
out vec3 tangent;
out vec3 bitangent;
out vec3 normal;
out vec2 texcoord;
out vec3 viewVec;
out vec4 smTexcoord;
out vec3 smTexcoord2;
out vec3 terrainCenter;

void main() {

    // just print the depth map to the screen
    if(false) {
        texcoord = vt;
        gl_Position = vec4(vp, 0.0, 1.0);
    }

    // draw scene as normal
    if(true) {
        // transform tangents and normal
        tangent = normalize(mat3(modelViewMatrix) * dPdu);
        bitangent = normalize(mat3(modelViewMatrix) * dPdv);
        normal = normalize(vNormal * mat3(modelViewInverse));

        // position in view space
        vec4 pos = modelViewMatrix * vec4(vPosition, 1);
        position = pos.xyz / pos.w;

        // light position in view space
        vec3 lightpos_real;
        lightpos_real.x = lightpos.y;
        lightpos_real.y = lightpos.x;
        lightpos_real.z = lightpos.z;

        vec4 lpos = modelViewMatrix * vec4(lightpos_real, 1);
        light = lpos.xyz / lpos.w;

        // pass through texture coordinate
        texcoord = vUV;

        // create a view vector in tangent space
        mat3 TBNmat = mat3(tangent, bitangent, normal);
        viewVec = -position * TBNmat;

        // for other tangent space transformation (if needed)
        vec4 Pv = modelViewMatrix * vec4(position, 0);
        mat4 TBNPmat = mat4(vec4(tangent, 0),
                            vec4(bitangent, 0),
                            vec4(normal, 0),
                            Pv);

        // shadowmap texture coordinates
        vec4 smpos = lightViewMat * vec4(vPosition, 1);
        smTexcoord = (biasMat * lightProjMat) * smpos;

        // divide by w to
        smTexcoord2 = smTexcoord.xyz / smTexcoord.w;

        // center of terrain
        vec4 terrainCenter4 = modelViewMatrix * vec4(256.0, 256.0, -25.0f, 1);
        terrainCenter = terrainCenter4.xyz / terrainCenter4.w;
    
        // rendering position
        gl_Position = projectionMatrix * pos;
    }
}
