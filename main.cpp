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
#include <chrono>

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

    //Main loop
    std::chrono::duration<double> t(0.0);
    std::chrono::duration<double> dt(0.01);
    std::chrono::duration<double> accumulator(0.0);

    std::chrono::time_point<std::chrono::system_clock> currentTime, newTime;
    currentTime = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        newTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = newTime - currentTime;
        currentTime = newTime;
        accumulator += elapsed_seconds;

        //Simulation
        while (accumulator >= dt)
        {
            g_game.SimulationStep(dt.count());
            accumulator -= dt;
            t += dt;
        }

        //Render
        {
            TRACE("Render");
            //If user has resized window, update viewport and projection
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            //Draw
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            g_game.Render(width, height);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    //Main loop has exited, clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
