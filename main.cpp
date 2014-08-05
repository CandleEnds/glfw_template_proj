

#include "glHeaders.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "loadShader.h"

#include <iostream>
#include <vector>

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
    //Initialize GLFW
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

    #ifdef WIN32
    //Initialize GLEW
    glewExperimental=true;
    if (glewInit() != GLEW_OK)
    {
    	fprintf(stderr, "Failed to initialize GLEW\n");
    	glfwDestroyWindow(window);
    	glfwTerminate();
    	exit(EXIT_FAILURE);
    }
    #endif

    // Create all of the buffers
    std::vector<GLfloat> vertices = {
    	-1.0f, -1.0f, 0.0f,
    	1.0f, -1.0f, 0.0f,
    	1.0f, 1.0, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -5.0f,
        1.0f, -1.0f, -5.0f,
        1.0f, 1.0f, -5.0f,
        -1.0f, 1.0f, -5.0f
    };
    std::vector<GLuint> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7
    };

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(GLfloat), 
        &vertices[0], 
        GL_STATIC_DRAW);

    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        indices.size() * sizeof(GLuint), 
        &indices[0], 
        GL_STATIC_DRAW);

    // Load shaders
    GLuint programID = LoadShader("../shaders/vert_basic.glsl", "../shaders/frag_basic.glsl");

    // Set up transformation matrices
    glm::mat4 Projection;
    glm::mat4 View = glm::lookAt(
        glm::vec3(0, 0, 5),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0));
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP;

    std::cout << "Beginning main render loop" << std::endl;
    //Main render loop
    while (!glfwWindowShouldClose(window))
    {
        //If user has resized window, update frame buffer and projection
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        ratio = (float)width / (float)height;
        //Projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
        float orthoHeight = 10.f;

        float top = orthoHeight / 2.f;
        float bottom = -top;
        float left = bottom * ratio;
        float right = -left;
        Projection = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);
        MVP = Projection * View * Model;

        //Draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        //Set shader variables
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        GLuint vertexPosLoc = glGetAttribLocation(programID, "vertexPos");

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glVertexAttribPointer(
            vertexPosLoc, //Attrib 0
            3, //size
            GL_FLOAT, //type
            GL_FALSE, //normalized?
            0, //stride
            (void*)0 //arraybuf offset
        );
        glDrawElements(GL_TRIANGLES,
            indices.size(),
            GL_UNSIGNED_INT,
            (void*)0);

    	glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
