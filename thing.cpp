#include "thing.h"

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

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, 
        m_aVertices.size() * sizeof(GLfloat), 
        &m_aVertices[0], 
        GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        m_aIndices.size() * sizeof(GLuint), 
        &m_aIndices[0], 
        GL_STATIC_DRAW);
}

b2Vec2 Thing::GetPosition() const
{
    return b2Vec2(m_pBody->GetPosition());
}
