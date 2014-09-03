#include "glHeaders.h"
#include "loadShader.h"

#include <GLFW/glfw3.h>

#include <Box2D/Box2D.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>

#include <iostream>
#include <sstream>
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

b2Body* createGroundBody(b2World& world)
{
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);
    return groundBody;
}

b2Body* createDynamicBody(b2World& world)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    b2Body* body = world.CreateBody(&bodyDef);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    return body;
}

class Thing
{
public:
    Thing(b2Body* pBody);
    b2Vec2 GetPosition() const;

    b2Body* m_pBody;
    std::vector<GLfloat> m_aVertices;
    std::vector<GLuint> m_aIndices;
};

Thing::Thing(b2Body* pBody)
: m_pBody(pBody)
{
    b2Fixture* pFix = pBody->GetFixtureList();
    while (pFix != NULL)
    {
        b2Shape* pShape = pFix->GetShape();
        if (b2PolygonShape* pPoly = dynamic_cast<b2PolygonShape*>(pShape))
        {
            assert(pPoly->GetVertexCount() == 4 && "Can only handle boxes right now thx");
            for (size_t i=0; i < 4; i++)
            {
                const b2Vec2& pos = pPoly->GetVertex(i);
                TRACE("Box pos " << pos.x << " " << pos.y << " " << 0.f);
                m_aVertices.push_back(pos.x);
                m_aVertices.push_back(pos.y);
                m_aVertices.push_back(0.f);
            }
            size_t start = m_aIndices.size();
            m_aIndices.push_back(start + 0);
            m_aIndices.push_back(start + 1);
            m_aIndices.push_back(start + 2);
            m_aIndices.push_back(start + 0);
            m_aIndices.push_back(start + 2);
            m_aIndices.push_back(start + 3);
        }
        pFix = pFix->GetNext();
    }
}

b2Vec2 Thing::GetPosition() const
{
    return b2Vec2(m_pBody->GetPosition());
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

    // Initialize Box2D 
    b2Vec2 gravity(0.0f, -10.0f);
    b2World world(gravity);
    b2Body* groundBody = createGroundBody(world);
    b2Body* dynamicBody = createDynamicBody(world);
    Thing thing(dynamicBody);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, 
        thing.m_aVertices.size() * sizeof(GLfloat), 
        &thing.m_aVertices[0], 
        GL_STATIC_DRAW);

    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        thing.m_aIndices.size() * sizeof(GLuint), 
        &thing.m_aIndices[0], 
        GL_STATIC_DRAW);

    // Load shaders
    GLuint programID = LoadShader("../shaders/vert_basic.glsl", "../shaders/frag_basic.glsl");

    // Set up transformation matrices
    glm::mat4 Projection;
    glm::mat4 View = glm::lookAt(
        glm::vec3(0, 0, 5),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0));
    glm::mat4 Model;
    glm::mat4 MVP;

    const float timeStep = 1.0f / 60.0f;
    const size_t velocityIterations = 6;
    const size_t positionIterations = 2;

    //Main render loop
    while (!glfwWindowShouldClose(window))
    {
        //Simulate
        world.Step(timeStep, velocityIterations, positionIterations);

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

        // Get model matrix from box2d body
        Model = glm::mat4(1.0f);
        Model = glm::rotate(Model, 0.f /*angle*/, glm::vec3(0.f,0.f,1.f));
        b2Vec2 modelPos = thing.GetPosition();
        Model = glm::translate(Model, glm::vec3(modelPos.x, modelPos.y, 0));

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
            thing.m_aIndices.size(),
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
