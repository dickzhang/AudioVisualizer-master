#include "RectShape.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

RectShape::RectShape()
{

}

RectShape::~RectShape()
{

}

bool RectShape::Init()
{
	shader = LoadShaders("Shaders/AudioRect.vs","Shaders/AudioRect.fs");
	if(shader>0)
	{
		MVPID = glGetUniformLocation(shader,"MVP");
	}
	return true;
}

void RectShape::Draw(Visualizer* visualizer)
{
	auto heightlist = visualizer->GetHeightList(m_Framecount%m_TotalNum);
	int average = 10;
	vector<float> templist;
	float tempnum = 0;
	for(int i = 0; i<heightlist.size(); i++)
	{
		auto temp = i%average;
		if(temp==0&&i>0)
		{
			templist.push_back(tempnum/average);
			tempnum = 0;
		}
		else
		{
			tempnum += heightlist[i];
		}
		if(i==heightlist.size()-1)
		{
			templist.push_back(tempnum/(temp+1));
		}
	}

	auto vao = GenVAO(templist);
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f),1280.0f/720.0f,0.1f,1000.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0,0,0),
		glm::vec3(0,0,-1),
		glm::vec3(0,1,0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::translate(Model,glm::vec3(-5,0,-10));
	glm::mat4 MVP = Projection*View*Model;
	glClearColor(0.3,0.3,0.3,1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,heightlist.size()*6);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}

unsigned int RectShape::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	std::vector<glm::vec3> vertices = GetVetexData(heigthlist);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

std::vector<glm::vec3> RectShape::GetVetexData(const std::vector<float>& heigthlist)
{
	std::vector<glm::vec3> vertexdata;
	if(heigthlist.empty())return vertexdata;
	float lenght = 10.0;
	float Hfactor = 5.0;
	float dtx = 0.25;
	float segment = 0.25;
	for(int i = 0; i<heigthlist.size(); i++)
	{
		glm::vec3 p0 = { i*segment+i*dtx,0,0 };
		glm::vec3 color0 = { 1,0,0.0 };

		glm::vec3 p1 = { (i+1)*segment+i*dtx,0,0 };
		glm::vec3 color1 = { 1,0,0.0 };

		glm::vec3 p2 = { (i+1)*segment+i*dtx,heigthlist[i]*Hfactor,0 };
		glm::vec3 color2 = { 1.0,0,0 };

		glm::vec3 p3 = { i*segment+i*dtx,heigthlist[i]*Hfactor,0 };
		glm::vec3 color3 = { 1.0,0,0 };

		vertexdata.push_back(p0);
		vertexdata.push_back(color0);
		vertexdata.push_back(p1);
		vertexdata.push_back(color1);
		vertexdata.push_back(p2);
		vertexdata.push_back(color2);
		vertexdata.push_back(p2);
		vertexdata.push_back(color2);
		vertexdata.push_back(p3);
		vertexdata.push_back(color3);
		vertexdata.push_back(p0);
		vertexdata.push_back(color0);
	}
	return vertexdata;
}