#include "thing.h"

#include <iostream>

#include <GLFW/glfw3.h>

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
    if (!m_aVertices.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER,
            m_aVertices.size() * sizeof(GLfloat),
            &m_aVertices[0],
            GL_STATIC_DRAW);
    }

    glGenBuffers(1, &indexBufferID);
    if (!m_aIndices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_aIndices.size() * sizeof(GLuint),
            &m_aIndices[0],
            GL_STATIC_DRAW);
    }
    glGenBuffers(1, &edgeIndexBufferID);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeIndexBufferID);
    if (!m_aEdgeIndices.empty())
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_aEdgeIndices.size() * sizeof(GLuint),
            &m_aEdgeIndices[0],
            GL_STATIC_DRAW);
    }
}

b2Vec2 Thing::GetPosition() const
{
    return b2Vec2(m_pBody->GetPosition());
}

Player::Player(b2World& world)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.f, 4.f);
    bodyDef.fixedRotation = true;
    b2Body* dynamicBody = world.CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.8f, 1.f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 10.0f;
    dynamicBody->CreateFixture(&fixtureDef);

    b2PolygonShape playerLeft;
    playerLeft.SetAsBox(0.1f, 0.8f, b2Vec2(-0.9f, 0.f), 0.f);
    fixtureDef.shape = &playerLeft;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.f;
    dynamicBody->CreateFixture(&fixtureDef);

    b2PolygonShape playerRight;
    playerRight.SetAsBox(0.1f, 0.8f, b2Vec2(0.9f, 0.f), 0.f);
    fixtureDef.shape = &playerRight;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.f;
    dynamicBody->CreateFixture(&fixtureDef);

	m_thing = std::make_shared<Thing>(dynamicBody);
}
    
void Player::KeyHandler(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_A:
            m_XVel -= m_speed;
            break;
        case GLFW_KEY_D:
            m_XVel += m_speed;
            break;
        case GLFW_KEY_W:
            m_thing->m_pBody->ApplyForceToCenter(m_jumpForce * b2Vec2(0, 1), true);
            break;
        default:
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_A:
            m_XVel += m_speed;
            break;
        case GLFW_KEY_D:
            m_XVel -= m_speed;
            break;
        default:
            break;
        }
    }
}

void Player::PreSimulation(float dtSeconds)
{
    const b2Vec2& playerVel = m_thing->m_pBody->GetLinearVelocity();
    if ((m_XVel < 0 && playerVel.x > -m_XVelMax) ||
        (m_XVel >= 0 && playerVel.x < m_XVelMax))
    {
        m_thing->m_pBody->ApplyForceToCenter(b2Vec2(m_XVel, 0), true);
    }
}

void Player::PostSimulation()
{

    b2ContactEdge* pContact = m_thing->m_pBody->GetContactList();
    int i = 0;
    while (pContact != NULL)
    {
        TRACE("Contact " << i++);
        pContact = pContact->next;
    }
}
