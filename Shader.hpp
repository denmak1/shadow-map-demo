// shader loading
#ifndef Shader_hpp
#define Shader_hpp

// info we need to load a single shader
struct ShaderInfo {
    unsigned int id;            // shader object ID
    const char *file;           // file to load into this object
};

// load shader from file into id = existing shader object
// shader type is defined by shader object type
// return false on compile error
bool loadShader(unsigned int id, const char *file);

// load a set of shaders
// progID is the program object
// components[numComponents] is a list of shader components to link
// return false on compile error
bool loadShaders(unsigned int progID, 
                 unsigned int numComponents,
                 ShaderInfo *components);


// load set of shaders
#endif
