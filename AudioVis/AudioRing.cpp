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

void AudioRing::Draw(const AudioObject& audioObject, const Visualizer& visualizer)
{
	auto vao = GenVAO(audioObject.GetHeightList());
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
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glDeleteVertexArrays(1, &vao);
}

unsigned int AudioRing::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	GetVetexData(heigthlist);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_Vertexdata.size(), &m_Vertexdata[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

void AudioRing::GetVetexData(const std::vector<float>& heigthlist)
{
	m_Vertexdata.clear();
	m_Indices.clear();

	m_Vertexdata.push_back({ 0,0,0 });
	m_Vertexdata.push_back({ 0.0,1.0,0.0 });
	int pointSide = heigthlist.size() + 0.5f;
	auto startRadian = glm::radians(0.0f);
	auto endRadian = glm::radians(360.0f);
	float row_delta = (startRadian - endRadian) / pointSide;

	for (int i = 0; i < pointSide + 1; i++)
	{
		float m_fRadius = 1.0;
		if (i < heigthlist.size())
		{
			m_fRadius += 2 * heigthlist[i];
		}
		auto cosV = glm::cos(startRadian + i * row_delta);
		auto sinV = glm::sin(startRadian + i * row_delta);
		glm::vec3 position = glm::vec3(m_fRadius * cosV, m_fRadius * sinV, 0.0f);
		m_Vertexdata.push_back(position);
		glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
		m_Vertexdata.push_back(color);
	}
	for (int i = 1; i < pointSide + 2; i++)
	{
		m_Indices.push_back(0);
		m_Indices.push_back(i + 1);
		m_Indices.push_back(i);
	}
}