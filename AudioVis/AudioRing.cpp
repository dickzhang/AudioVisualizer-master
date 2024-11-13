#include "AudioRing.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

AudioRing::AudioRing()
{

}

AudioRing::~AudioRing()
{

}

bool AudioRing::Init()
{
	shader = LoadShaders("Shaders/AudioRect.vs", "Shaders/AudioRect.fs");
	if (shader > 0)
	{
		MVPID = glGetUniformLocation(shader, "MVP");
	}
	return true;
}

void AudioRing::Draw(Visualizer* visualizer)
{
	auto heightlist = visualizer->GetHeightList(m_Framecount%m_TotalNum);
	auto vao = GenVAO(heightlist);
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::translate(Model, glm::vec3(0, 0, -10));
	glm::mat4 MVP = Projection * View * Model;
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0,heightlist.size() * 6);
	glDeleteVertexArrays(1, &vao);
	m_Framecount++;
}

unsigned int AudioRing::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	GetVetexData(heigthlist);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_Vertexdata.size(), &m_Vertexdata[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

void AudioRing::GetVetexData(const std::vector<float>& heigthlist)
{
	m_Vertexdata.clear();

	int pointSide = heigthlist.size() + 0.5f;
	auto startRadian = glm::radians(0.0f);
	auto endRadian = glm::radians(360.0f);
	float row_delta = (startRadian - endRadian) / pointSide;
	float delta = 0.015;
	for (int i = 0; i < pointSide + 1; i++)
	{
		float m_fRadius = 1.0;
		float m_fRadiusExtend = 1.0;
		if (i < heigthlist.size())
		{
			m_fRadiusExtend += 2 * heigthlist[i];
		}
		auto cosV = glm::cos(startRadian + i * row_delta);
		auto sinV = glm::sin(startRadian + i * row_delta);
		glm::vec3 position = glm::vec3(m_fRadiusExtend * cosV, m_fRadiusExtend * sinV, 0.0f);
		glm::vec3 pos = glm::vec3(m_fRadius * cosV, m_fRadius * sinV, 0.0f);
		glm::vec3 qieLine = { -pos.y,pos.x,0.0 };
		glm::vec3 norqeiline= glm::normalize(qieLine);
		glm::vec3 p0=pos - norqeiline * delta;
		glm::vec3 p1=pos + norqeiline * delta;
		glm::vec3 p2= position + norqeiline * delta;
		glm::vec3 p3= position - norqeiline * delta;

		glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
		m_Vertexdata.push_back(p0);
		m_Vertexdata.push_back(color);

		m_Vertexdata.push_back(p1);
		m_Vertexdata.push_back(color);

		m_Vertexdata.push_back(p2);
		m_Vertexdata.push_back(color);

		m_Vertexdata.push_back(p2);
		m_Vertexdata.push_back(color);

		m_Vertexdata.push_back(p3);
		m_Vertexdata.push_back(color);

		m_Vertexdata.push_back(p0);
		m_Vertexdata.push_back(color);
	}
}