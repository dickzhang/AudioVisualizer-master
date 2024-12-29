#include "RingRectShape.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

RingRectShape::RingRectShape()
{

}

RingRectShape::~RingRectShape()
{

}

bool RingRectShape::Init()
{
	shader = LoadShaders("Shaders/AudioRect.vs","Shaders/AudioRect.fs");
	if(shader>0)
	{
		MVPID = glGetUniformLocation(shader,"MVP");
	}
	return true;
}

void RingRectShape::Draw(Visualizer* visualizer)
{
	auto heightlist = visualizer->GetHeightList(m_Framecount%m_TotalNum);
	int averageNum = 2;
	vector<float> templist;
	for(int i = averageNum; i<heightlist.size(); i++)
	{
		float temp = 0;
		for(int k = 0; k<averageNum; k++)
		{
			temp += heightlist[i-k];
		}
		temp /= (averageNum+1);
		templist.push_back(temp);
	}
	auto vao = GenVAO(templist);
	//auto vao = GenVAO(heightlist);
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
	glDrawArrays(GL_TRIANGLES,0,heightlist.size()*6);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}

unsigned int RingRectShape::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);

	GetVetexData(heigthlist);

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*m_Vertexdata.size(),&m_Vertexdata[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

void RingRectShape::GetVetexData(const std::vector<float>& heigthlist)
{
	m_Vertexdata.clear();
	int num=heigthlist.size()*0.8;
	float row_delta = -glm::radians(360.0f)/num;
	float delta = 0.02;
	glm::vec3 color = { 254.0/255.0,164/255.0,67/255.0 };
	for(int i = 0; i<num; i++)
	{
		float m_fRadius = 3.0;
		auto cosV = glm::cos(i*row_delta);
		auto sinV = glm::sin(i*row_delta);
		float len = heigthlist[i];
		if(len<0.05)
		{
			len = 0.05;
		}
		glm::vec3 positionW = glm::vec3((m_fRadius+len)*cosV,(m_fRadius+len)*sinV,0.0f);
		glm::vec3 positionN = glm::vec3((m_fRadius-len)*cosV,(m_fRadius-len)*sinV,0.0f);
		glm::vec3 qieLine = { -positionN.y,positionN.x,0.0 };
		glm::vec3 norqeiline = glm::normalize(qieLine);
		glm::vec3 p0 = positionN-norqeiline*delta;
		glm::vec3 p1 = positionN+norqeiline*delta;
		glm::vec3 p2 = positionW+norqeiline*delta;
		glm::vec3 p3 = positionW-norqeiline*delta;

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