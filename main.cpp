#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental=true;
    if (glewInit() != GLEW_OK)
    {
    	fprintf(stderr, "Failed to initialize GLEW\n");
    	glfwDestroyWindow(window);
    	glfwTerminate();
    	exit(EXIT_FAILURE);
    }

    const GLfloat pVBufData[] = {
    	-1.0f, -1.0f, 0.0f,
    	1.0f, -1.0f, 0.0f,
    	0.0f, 1.0, 0.0f,
    };

    GLuint nVBuf;
    glGenBuffers(1, &nVBuf);
    glBindBuffer(GL_ARRAY_BUFFER, nVBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pVBufData), pVBufData, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, nVBuf);
        glVertexAttribPointer(
        	0, //Attrib 0
        	3, //size
        	GL_FLOAT, //type
        	GL_FALSE, //normalized?
        	0, //stride
        	(void*)0 //arraybuf offset
    	);
    	glDrawArrays(GL_TRIANGLES, 0, 3);
    	glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
