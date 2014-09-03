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
    std::vector<GLfloat> m_aVertices;
    std::vector<GLuint> m_aIndices;
    GLuint vertexBufferID;
    GLuint indexBufferID;
};