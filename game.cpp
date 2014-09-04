#include "game.h"

#include "loadShader.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/constants.hpp>

#include <math.h>


Game::Game()
: m_gravity(0.f, -10.f)
, m_world(m_gravity)
{
    m_rotation = glm::pi<float>() / 2;
    m_worldUp = b2Vec2(cos(m_rotation), sin(m_rotation));
}

void Game::Initialize()
{
	b2Body* groundBody = createStaticBox(0.f, -10.f, 50.f, 10.f);
	b2Body* dynamicBody = createDynamicBox(0.f, 4.f, 1.f, 1.f);
	m_staticThings.emplace_back(groundBody);
	m_player = std::make_shared<Thing>(dynamicBody);

    m_standardShader = LoadShader("../shaders/vert_basic.glsl", "../shaders/frag_basic.glsl");
}

void Game::KeyHandler(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_A)
		{
			m_player->m_pBody->ApplyForceToCenter(b2Vec2(-400,0), true);
		}
		else if (key == GLFW_KEY_D)
		{
			m_player->m_pBody->ApplyForceToCenter(b2Vec2(400,0), true);
		}
		else if (key == GLFW_KEY_W)
		{
			m_player->m_pBody->ApplyForceToCenter(1000 * m_worldUp, true);
		}
        else if (key == GLFW_KEY_LEFT)
        {
            m_rotation -= glm::pi<float>() / 8.0;
            m_worldUp = b2Vec2(cos(m_rotation), sin(m_rotation));
            m_world.SetGravity(10 * -m_worldUp);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            m_rotation += glm::pi<float>() / 8.0;
            m_worldUp = b2Vec2(cos(m_rotation), sin(m_rotation));
            m_world.SetGravity(10 * -m_worldUp);
        }

	}
}

void Game::SimulationStep(float dtSeconds)
{
	m_world.Step(dtSeconds, m_nVelocityIterations, m_nPositionIterations);
}

void Game::Render(int pixelWidth, int pixelHeight)
{
	glViewport(0, 0, pixelWidth, pixelHeight);

	float ratio = (float)pixelWidth / (float)pixelHeight;
    float orthoHeight = 15.f;
    float top = orthoHeight / 2.f;
    float bottom = -top;
    float left = bottom * ratio;
    float right = -left;
    glm::mat4 Projection = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);

    b2Vec2 pp = m_player->m_pBody->GetPosition();
    glm::mat4 View = glm::lookAt(
        glm::vec3(pp.x, pp.y, 5),
        glm::vec3(pp.x,pp.y,0),
        glm::vec3(m_worldUp.x, m_worldUp.y, 0));
    glm::mat4 VP = Projection * View;

    glUseProgram(m_standardShader);
    DrawThing(*m_player, VP);
    for (Thing& thing : m_staticThings)
    {
    	DrawThing(thing, VP);
    }
}

void Game::DrawThing(Thing& thing, glm::mat4& VP)
{
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::rotate(Model, 0.f /*angle*/, glm::vec3(0.f, 0.f, 1.f));
    b2Vec2 modelPos = thing.GetPosition();
    Model = glm::translate(Model, glm::vec3(modelPos.x, modelPos.y, 0));

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
