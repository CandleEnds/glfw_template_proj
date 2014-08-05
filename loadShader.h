#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

GLuint LoadShader(const char* vertex_shader_path, 
	const char* fragment_shader_path);