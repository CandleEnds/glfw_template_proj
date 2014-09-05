#include "game.h"

#include "loadShader.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/constants.hpp>

#include <math.h>


Game::Game()
: m_gravity(0.f, -30.f)
, m_world(m_gravity)
, m_angularSpeed(glm::pi<float>() / 4)
{
    m_worldUp = b2Vec2(0, 1);
}

void Game::Initialize()
{
    float bs = 16.f;
    float bw = 1.f;
    m_stageCenter = b2Vec2(0.f, bs);

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(m_stageCenter.x, m_stageCenter.y);
    b2Body* stageBody = m_world.CreateBody(&bodyDef);

    // b2PolygonShape box;
    // box.SetAsBox(bs, bw, b2Vec2(0, -bs), 0.f);
    // stageBody->CreateFixture(&box, 0.0f);

    // box.SetAsBox(bw, bs, b2Vec2(-bs, 0), 0.f);
    // stageBody->CreateFixture(&box, 0.0f);

    // box.SetAsBox(bw, bs, b2Vec2(bs, 0), 0.f);
    // stageBody->CreateFixture(&box, 0.f);

    // box.SetAsBox(bs, bw, b2Vec2(0, bs), 0.f);
    // stageBody->CreateFixture(&box, 0.f);

    std::vector<b2Vec2> verts = {
        b2Vec2(-bs, -bs),
        b2Vec2(bs, -bs),
        b2Vec2(bs, bs),
        b2Vec2(-bs, bs)
    };

    b2ChainShape chain;
    chain.CreateLoop(
        &verts[0],
        4
    );

    stageBody->CreateFixture(&chain, 0.f);

    m_stage = std::make_shared<Thing>(stageBody);

    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.f, 4.f);
    bodyDef.fixedRotation = true;
    b2Body* dynamicBody = m_world.CreateBody(&bodyDef);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.f, 1.f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 10.0f;
    dynamicBody->CreateFixture(&fixtureDef);

	m_player = std::make_shared<Thing>(dynamicBody);

    m_standardShader = LoadShader("../shaders/vert_basic.glsl", "../shaders/frag_basic.glsl");
}

void Game::KeyHandler(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_A)
		{
            m_playerXVel -= m_playerSpeed;
		}
		else if (key == GLFW_KEY_D)
		{
            m_playerXVel += m_playerSpeed;
		}
		else if (key == GLFW_KEY_W)
		{
			m_player->m_pBody->ApplyForceToCenter(60000 * m_worldUp, true);
		}
        else if (key == GLFW_KEY_LEFT)
        {
            m_stage->m_pBody->SetAngularVelocity(m_angularSpeed);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            m_stage->m_pBody->SetAngularVelocity(-m_angularSpeed);
        }
	}
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_A) 
        {
            m_playerXVel += m_playerSpeed;
        }
        else if (key == GLFW_KEY_D)
        {
            m_playerXVel -= m_playerSpeed;
        }
        else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
        {
            m_stage->m_pBody->SetAngularVelocity(0);
        }
    }
}

void Game::SimulationStep(float dtSeconds)
{
    const b2Vec2& playerVel = m_player->m_pBody->GetLinearVelocity();
    if ((m_playerXVel < 0 && playerVel.x > -m_playerXVelMax) ||
        (m_playerXVel >= 0 && playerVel.x < m_playerXVelMax))
    {
        m_player->m_pBody->ApplyForceToCenter(b2Vec2(m_playerXVel, 0), true);
    }
	m_world.Step(dtSeconds, m_nVelocityIterations, m_nPositionIterations);
}

void Game::Render(int pixelWidth, int pixelHeight)
{
	glViewport(0, 0, pixelWidth, pixelHeight);

	float ratio = (float)pixelWidth / (float)pixelHeight;
    float orthoHeight = 40.f;
    float top = orthoHeight / 2.f;
    float bottom = -top;
    float left = bottom * ratio;
    float right = -left;
    glm::mat4 Projection = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);

    //b2Vec2 pp = m_player->m_pBody->GetPosition();
    glm::mat4 View = glm::lookAt(
        glm::vec3(m_stageCenter.x, m_stageCenter.y, 5),
        glm::vec3(m_stageCenter.x, m_stageCenter.y, 0),
        glm::vec3(m_worldUp.x, m_worldUp.y, 0));
    glm::mat4 VP = Projection * View;

    glUseProgram(m_standardShader);
    DrawThing(*m_player, VP);
    DrawThing(*m_stage, VP);
}

void Game::DrawThing(Thing& thing, glm::mat4& VP)
{
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::rotate(Model, 0.f /*angle*/, glm::vec3(0.f, 0.f, 1.f));
    b2Vec2 modelPos = thing.GetPosition();

    Model = glm::translate(Model, glm::vec3(modelPos.x, modelPos.y, 0));
    Model = glm::rotate(Model, thing.m_pBody->GetAngle(), glm::vec3(0.f,0.f,1.f));

    glm::mat4 MVP = VP * Model; 

    //Get shader variables
    GLuint MatrixID = glGetUniformLocation(m_standardShader, "MVP");
    GLuint vertexPosLoc = glGetAttribLocation(m_standardShader, "vertexPos");

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, thing.vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thing.indexBufferID);
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
		(void*)0
	);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thing.edgeIndexBufferID);
    glDrawElements(GL_LINE_STRIP,
        thing.m_aEdgeIndices.size(),
        GL_UNSIGNED_INT,
        (void*)0
    );

	glDisableVertexAttribArray(0);
}

b2Body* Game::createStaticBox(float posX, float posY, float sizeX, float sizeY)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(posX, posY);
    b2Body* body = m_world.CreateBody(&bodyDef);
    b2PolygonShape box;
    box.SetAsBox(sizeX, sizeY);
    body->CreateFixture(&box, 0.0f);
    return body;
}

b2Body* Game::createDynamicBox(float posX, float posY, float sizeX, float sizeY)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(posX, posY);
    b2Body* body = m_world.CreateBody(&bodyDef);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(sizeX, sizeY);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    return body;
}
