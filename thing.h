#pragma once

#include "glHeaders.h"

#include <Box2D/Box2D.h>

#include <vector>

class Thing
{
public:
    Thing(b2Body* pBody);
    b2Vec2 GetPosition() const;

    b2Body* m_pBody;

    std::vector<GLuint> m_aIndices;
    std::vector<GLuint> m_aEdgeIndices;

    std::vector<GLfloat> m_aVertices;	

    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint edgeIndexBufferID;
};

class Player 
{
public:
    Player(b2World& world);
    void KeyHandler(int key, int scancode, int action, int mods);
    void PreSimulation(float dtSeconds);
    void PostSimulation();

    float m_XVel;
    const float m_XVelMax = 15.f;
    const float m_speed = 5000.f;
    const float m_jumpForce = 60000.f;
    std::shared_ptr<Thing> m_thing;
};