// functions to load shaders

#include "Shader.hpp"

// using core modern OpenGL
#ifdef _WIN32
#  include <GL/glew.h>
#else
#  define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include <stdio.h>

#ifdef _WIN32
// don't complain if we use standard IO functions instead of windows-only
#pragma warning( disable: 4996 )
#endif

//
// load and compile a single shader
// id is an existing shader object
// shader type is defined by shader object type
//
bool loadShader(unsigned int id, const char *file)
{
    // open file
    FILE *f = fopen(file, "rb");
    if (! f) {
        fprintf(stderr, "unable to open shader %s\n", file);
        return false;           // error
    }

    // get file size
    // seek to end of file is more cross-platform than fstat
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // read entire file and compile as shader
    GLchar *shader = new GLchar[size];
    fread(shader, 1, size, f);
    glShaderSource(id, 1, (const GLchar**)(&shader), &size);
    glCompileShader(id);
    delete[] shader;

    // report compile errors
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (! success) {
       // how big is the message?
        GLsizei infoLen;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);

        // print the message
        char *infoLog = new char[infoLen];
        glGetShaderInfoLog(id, infoLen, 0, infoLog);
        fprintf(stderr, "%s", infoLog);

        // free the message buffer
        delete[] infoLog;

        return false;           // error
     }

    return true;                // success
}


//
// load a set of shaders
//
bool loadShaders(unsigned int progID,
                 unsigned int numComponents,
                 ShaderInfo *components)
{
    // load shader code
    for(unsigned int i=0; i<numComponents; ++i) {
        if (! loadShader(components[i].id, components[i].file))
            return false;       // error
    }

    // link shader programs
    // don't attach until everything compiles successfully to avoid
    // extraneous attach affecting next attempt to link if there was an error
    for(unsigned int i=0; i<numComponents; ++i)
        glAttachShader(progID, components[i].id);
    glLinkProgram(progID);

    // report link errors
    GLint success;
    glGetProgramiv(progID, GL_LINK_STATUS, &success);
    if (! success) {
        // how big is the message?
        GLsizei infoLen;
        glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &infoLen);

        // print the message
        char *infoLog = new char[infoLen];
        glGetProgramInfoLog(progID, infoLen, 0, infoLog);
        fprintf(stderr, "%s", infoLog);

        // free the message buffer
        delete[] infoLog;
        return false;           // error
    }

    return true;                // success
}
