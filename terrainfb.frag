// 2nd pass post processing fragment shader
#version 400 core

uniform sampler2D fbotex;
uniform float range;

// input from vertex shader
in vec2 texcoord;

// output to frame buffer
out vec4 fragColor;

vec4 bloom();

void main() {
    // just draw the texture to the screen   
    //fragColor = texture(fbotex, texcoord);

    // apply HDR bloom effect
    fragColor = bloom();
}

vec4 bloom() {
    fragColor = texture(fbotex, texcoord);
    vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vec2 neighbortex;

    for(int i = -4; i < 4; i++) {
        for(int j = -3; j < 3; j++) {
            neighbortex = texcoord + (vec2(j, i) * range);
            sum += texture2D(fbotex, neighbortex) * 0.25f;
        }
    }

    if(texture2D(fbotex, texcoord).r <= 0.35)
        fragColor = 0.011f * (sum*sum) + texture2D(fbotex, texcoord);
    else {
        if (texture2D(fbotex, texcoord).r <= 0.5)
            fragColor = 0.009f * (sum*sum) + texture2D(fbotex, texcoord);
        else
            fragColor = 0.0079f * (sum*sum) + texture2D(fbotex, texcoord);
    }

    

    return fragColor;
}