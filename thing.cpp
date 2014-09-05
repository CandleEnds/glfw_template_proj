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
            size_t start = m_aVertices.size() / 3;

            assert(pPoly->GetVertexCount() == 4 && "Can only handle boxes right now thx");
            for (size_t i=0; i < 4; i++)
            {
                const b2Vec2& pos = pPoly->GetVertex(i);
                m_aVertices.push_back(pos.x);
                m_aVertices.push_back(pos.y);
                m_aVertices.push_back(0.f);
            }
            
            m_aIndices.push_back(start + 0);
            m_aIndices.push_back(start + 1);
            m_aIndices.push_back(start + 2);
            m_aIndices.push_back(start + 0);
            m_aIndices.push_back(start + 2);
            m_aIndices.push_back(start + 3);
        }
        else if (b2ChainShape* pChain = dynamic_cast<b2ChainShape*>(pShape))
        {
            size_t start = m_aVertices.size() / 3;
            TRACE("Chain children " << pChain->GetChildCount());

            size_t numVerts = pChain->m_count;
            for (size_t i = 0; i < numVerts; i++)
            {
                const b2Vec2& pos = pChain->m_vertices[i];
                m_aVertices.push_back(pos.x);
                m_aVertices.push_back(pos.y);
                m_aVertices.push_back(0.f);
                m_aEdgeIndices.push_back(start++);
            }
        }

        pFix = pFix->GetNext();
    }

    for (size_t i = 0; i < m_aEdgeIndices.size(); i++)
    {
        std::cout << m_aEdgeIndices[i] << " ";
    }
    std::cout << std::endl;

    assert(m_aVertices.size() % 3 == 0);

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

    glGenBuffers(1, &edgeIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        m_aEdgeIndices.size() * sizeof(GLuint),
        &m_aEdgeIndices[0],
        GL_STATIC_DRAW);
}

b2Vec2 Thing::GetPosition() const
{
    return b2Vec2(m_pBody->GetPosition());
}
