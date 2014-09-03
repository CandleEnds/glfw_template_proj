#include "glHeaders.h"
#include "loadShader.h"
#include "game.h"

#include <GLFW/glfw3.h>

#include <Box2D/Box2D.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>

#include <iostream>
#include <sstream>
#include <vector>

Game g_game;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else
        g_game.KeyHandler(key, scancode, action, mods);
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
    glewExperimental = GL_TRUE;
    GLenum glewInitStatus = glewInit();
    if (glewInitStatus != GLEW_OK)
    {
        TRACE("Glew Init Error: " << glewGetErrorString(glewInitStatus));
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#endif

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    g_game.Initialize();

    // Set up transformation matrices
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 ViewProjection;

    const float timeStep = 1.0f / 60.0f;

    //Main render loop
    while (!glfwWindowShouldClose(window))
    {
        //Simulate
        g_game.SimulationStep(timeStep);

        //If user has resized window, update viewport and projection
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        ratio = (float)width / (float)height;

        float orthoHeight = 10.f;
        float top = orthoHeight / 2.f;
        float bottom = -top;
        float left = bottom * ratio;
        float right = -left;
        Projection = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);

        b2Vec2 pp = g_game.m_player->m_pBody->GetPosition();
        View = glm::lookAt(
            glm::vec3(pp.x, pp.y, 5),
            glm::vec3(pp.x,pp.y,0),
            glm::vec3(0,1,0));

        ViewProjection = Projection * View;

        //Draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_game.Render(ViewProjection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
