#if defined(__APPLE__)   
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#elif defined(WIN32)
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glew.h>
#endif