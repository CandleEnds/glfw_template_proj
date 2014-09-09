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
    m_stageCenter = b2Vec2(0.f, 16.f);

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(m_stageCenter.x, m_stageCenter.y);
    b2Body* stageBody = m_world.CreateBody(&bodyDef);

    const size_t numVerts = 32;
    std::vector<b2Vec2> verts;
    verts.reserve(numVerts);
    float stageRadius = 24.f;

    float twopi = 2 * glm::pi<float>();
    float increment = twopi / numVerts;
    for (int i = 0; i < numVerts; i++)
    {
        float angle = increment * i;
        verts.push_back(stageRadius * b2Vec2(cos(angle), sin(angle)));
    }

    b2ChainShape chain;
    chain.CreateLoop(
        &verts[0],
        verts.size()
    );

    stageBody->CreateFixture(&chain, 0.f);

    m_stage = std::make_shared<Thing>(stageBody);

    m_player = std::make_shared<Player>(m_world);

    m_standardShader = LoadShader("../shaders/vert_basic.glsl", "../shaders/frag_basic.glsl");
}

void Game::KeyHandler(int key, int scancode, int action, int mods)
{
    bool handled = true;
	if (action == GLFW_PRESS)
	{
        if (key == GLFW_KEY_LEFT)
        {
            m_stage->m_pBody->SetAngularVelocity(m_angularSpeed);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            m_stage->m_pBody->SetAngularVelocity(-m_angularSpeed);
        }
        else
        {
            handled = false;
        }
	}
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
        {
            m_stage->m_pBody->SetAngularVelocity(0);
        }
        else
        {
            handled = false;
        }
    }

    if (!handled)
    {
        m_player->KeyHandler(key, scancode, action, mods);
    }
}

void Game::SimulationStep(float dtSeconds)
{
    m_player->PreSimulation(dtSeconds);

	m_world.Step(dtSeconds, m_nVelocityIterations, m_nPositionIterations);
    
    m_player->PostSimulation();

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

    b2Vec2 pp = m_player->m_thing->m_pBody->GetPosition();
    glm::mat4 View = glm::lookAt(
        glm::vec3(pp.x, pp.y, 5),
        glm::vec3(pp.x, pp.y, 0),
        glm::vec3(m_worldUp.x, m_worldUp.y, 0));
    glm::mat4 VP = Projection * View;

    glUseProgram(m_standardShader);
    DrawThing(*m_player->m_thing, VP);
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

