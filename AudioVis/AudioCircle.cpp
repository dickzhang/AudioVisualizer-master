#include "AudioCircle.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#define DRAW_LINES 0

AudioCircle::AudioCircle()
{

}

AudioCircle::~AudioCircle()
{

}

bool AudioCircle::Init()
{
	shader = LoadShaders("Shaders/AudioRect.vs","Shaders/AudioRect.fs");
	if(shader>0)
	{
		MVPID = glGetUniformLocation(shader,"MVP");
	}
	return true;
}

void AudioCircle::Draw(Visualizer* visualizer)
{
	auto vao = GenVAO(visualizer->GetHeightList(m_Framecount%m_TotalNum));
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f),1280.0f/720.0f,0.1f,1000.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0,0,0),
		glm::vec3(0,0,-1),
		glm::vec3(0,1,0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::translate(Model,glm::vec3(0,0,-10));
	glm::mat4 MVP = Projection*View*Model;
	glClearColor(0.3,0.3,0.3,1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
	glBindVertexArray(vao);

#if DRAW_LINES
	glDrawElements(GL_LINE_STRIP,m_Indices.size(),GL_UNSIGNED_INT,0);
#else
	glDrawElements(GL_TRIANGLES,m_Indices.size(),GL_UNSIGNED_INT,0);
#endif // DRAW_LINES

	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}

unsigned int AudioCircle::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO,EBO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	glBindVertexArray(VAO);

	GetVetexData(heigthlist);

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*m_Vertexdata.size(),&m_Vertexdata[0],GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*m_Indices.size(),&m_Indices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

void AudioCircle::GetVetexData(const std::vector<float>& heigthlist)
{
	m_Vertexdata.clear();
	m_Indices.clear();
#if !DRAW_LINES
	m_Vertexdata.push_back({ 0,0,0 });
	m_Vertexdata.push_back({ 0.0,1.0,0.0 });
#endif
	int pointSide = heigthlist.size()+0.5f;
	auto startRadian = glm::radians(0.0f);
	auto endRadian = glm::radians(360.0f);
	float row_delta = (startRadian-endRadian)/pointSide;

	for(int i = 0; i<pointSide+1; i++)
	{
		float m_fRadius = 1.0;
		if(i<heigthlist.size())
		{
			m_fRadius += 2*heigthlist[i];
		}
		auto cosV = glm::cos(startRadian+i*row_delta);
		auto sinV = glm::sin(startRadian+i*row_delta);
		glm::vec3 position = glm::vec3(m_fRadius*cosV,m_fRadius*sinV,0.0f);
		m_Vertexdata.push_back(position);
		glm::vec3 color = glm::vec3(1.0,1.0,1.0);
		m_Vertexdata.push_back(color);
#if DRAW_LINES
		m_Indices.push_back(i);
#endif // DRAW_LINES
	}

#if !DRAW_LINES
	for(int i = 1; i<pointSide+2; i++)
	{
		m_Indices.push_back(0);
		m_Indices.push_back(i+1);
		m_Indices.push_back(i);
	}
#else
	m_Vertexdata.push_back(m_Vertexdata[0]);
	m_Vertexdata.push_back(glm::vec3(1.0,1.0,1.0));
	m_Indices.push_back(m_Indices.size());
#endif // DRAW_LINES
}