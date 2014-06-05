#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// fragment shader for simple terrain application
#version 400 core

// load textures
uniform sampler2D shadowMap;
uniform sampler2D colorTexture;
uniform sampler2D bumpMap;
uniform sampler2D glossMap;
uniform sampler2D heightMap;

// input from vertex shader
in vec3 position;
in vec3 light;
in vec3 tangent;
in vec3 bitangent;
in vec3 normal;
in vec2 texcoord;
in vec3 viewVec;
in vec4 smTexcoord;
in vec3 smTexcoord2;
in vec3 terrainCenter;

// output to frame buffer
out vec4 fragColor;

float calcVisibility();

void main() {

    // normalize set
    if(false) {
        float zNear = 0.05;
        float zFar = 100.0;

        float fragdepth = (2.0 * zNear) /
            (zFar + zNear - smTexcoord2.z * (zFar - zNear));

        fragColor = vec4(fragdepth);
    }

    // gl_FragCoord set
    if(false) {
        float zNear = 0.05;
        float zFar = 100.0;

        float fragdepth = (2.0 * zNear) /
            (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

        fragColor = vec4(fragdepth);
    }

    // working set
    if(true) {
        // standard stuff
        vec3 V = normalize(0 - position);        // view direction
        vec3 N = normalize(normal);              // normals for light
        
        // apply lighting and calculate diffuse from new normals
        vec3 L = normalize(light - position);    // light direction
        vec3 H = normalize(V + L);               // between view/light
        float diff = max(0.0, dot(N, L));        // diffuse lighting

        // just use standard white texture
        vec3 color = vec3(1.0, 1.0, 1.0);

        // compare depth values if needed, output darkened frag as color
        float visibility = calcVisibility();
        color = vec3(visibility);

        // final color
        fragColor = vec4(color*diff, 1.0);
    }

    // working set
    if(false) {
        // extract bump map and build new normals
        vec3 normalMap = texture2D(bumpMap, texcoord).xyz*2.0f - 1.0f;
        normalMap.xy *= 2;                       // makes bumps stronger
        vec3 N = normalize(normalMap.x * tangent
                         + normalMap.y * bitangent
                         + normalMap.z * normal);

        // standard stuff
        vec3 V = normalize(0 - position);        // view direction
        
        // apply lighting and calculate diffuse from new normals
        vec3 L = normalize(light - position);    // light direction
        vec3 H = normalize(V + L);               // between view/light
        float diff = max(0.0, dot(N, L));        // diffuse lighting

        // calculate specular from glossmap
        float glossy = pow(8192, texture2D(glossMap, texcoord).x);
        float blinnPhong = pow(max(0.0, dot(N, H)), glossy) * max(0.0, dot(N, L));
        float specular = (M_PI/8.0f) * blinnPhong;

        // fresnel
        float fresnel = 0.04f + pow(1 - max(0.0, dot(V,H)), 4);

        // color from texture
        vec3 color = texture(colorTexture, texcoord).rgb;

        // compare depth values if needed, output darkened frag as color
        float visibility = calcVisibility();     

        // apply specular and fesnel
        color = mix(color, vec3(specular), fresnel); 

        // final color
        fragColor = vec4(color*visibility*diff, 1.0);
    }

}

float calcVisibility() {
    float retVal = 1.0;
    
    // alternate way of calculating visibility
    if(false) {
        vec4 divided = smTexcoord / smTexcoord.w ;
        divided.z += 0.0005;

        float distFromLight = texture2D(shadowMap, divided.st).z;

        if(smTexcoord.w > 0.0)
            retVal = distFromLight < divided.z ? 0.5 : 1.0 ;
    }
    
    // standard way
    if(true) {
        float zNear = 0.05;
        float zFar = 100.0;

        float fragdepth1 = (2.0 * zNear) /
                     (zFar + zNear - smTexcoord2.z * (zFar - zNear));

        float bias = .003;
        if(texture(shadowMap, smTexcoord2.xy).z  < (fragdepth1 - bias))
            retVal = 0.5;
    }

    return retVal;
}