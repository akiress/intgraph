#include "stdafx.h"
#include "GLShader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

std::string readFile ( const char *filePath ) {
    std::string content;
    std::ifstream fileStream ( filePath, std::ios::in );

    if ( !fileStream.is_open () ) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while ( !fileStream.eof () ) {
        std::getline ( fileStream, line );
        content.append ( line + "\n" );
    }

    fileStream.close ();
    return content;
}

void shaderErrorMessages ( GLuint param )
{
    int logLength;
    glGetShaderiv ( param, GL_INFO_LOG_LENGTH, &logLength );
    // std::vector<char> shaderError ( logLength );
    std::vector<char> shaderError ( ( logLength > 1 ) ? logLength : 1 );
    glGetShaderInfoLog ( param, logLength, NULL, &shaderError[0] );
    std::cout << &shaderError[0] << std::endl;
}

GLuint LoadShader ( const char *vertex_path, const char *fragment_path ) {
    GLuint vertShader = glCreateShader ( GL_VERTEX_SHADER );
    GLuint fragShader = glCreateShader ( GL_FRAGMENT_SHADER );

    // Read shaders
    std::string vertShaderStr = readFile ( vertex_path );
    std::string fragShaderStr = readFile ( fragment_path );
    const char *vertShaderSrc = vertShaderStr.c_str ();
    const char *fragShaderSrc = fragShaderStr.c_str ();

    GLint status = GL_FALSE;

    // Compile vertex shader
    std::cout << "Compiling vertex shader." << std::endl;
    glShaderSource ( vertShader, 1, &vertShaderSrc, NULL );
    glCompileShader ( vertShader );
    glGetShaderiv ( vertShader, GL_COMPILE_STATUS, &status );
    if ( status == GL_FALSE ) shaderErrorMessages ( vertShader );

    // Compile fragment shader
    std::cout << "Compiling fragment shader." << std::endl;
    glShaderSource ( fragShader, 1, &fragShaderSrc, NULL );
    glCompileShader ( fragShader );
    glGetShaderiv ( fragShader, GL_COMPILE_STATUS, &status );
    if (status == GL_FALSE) shaderErrorMessages ( fragShader );

    std::cout << "Linking program" << std::endl;
    GLuint program = glCreateProgram ();
    glAttachShader ( program, vertShader );
    glAttachShader ( program, fragShader );
    glLinkProgram ( program );

    glGetProgramiv ( program, GL_LINK_STATUS, &status );
    if ( status == GL_FALSE ) shaderErrorMessages ( program );

    glDeleteShader ( vertShader );
    glDeleteShader ( fragShader );

    return program;
}