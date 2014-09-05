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