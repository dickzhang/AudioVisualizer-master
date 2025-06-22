#include "SpereShape.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <random>
#include <iostream>
#include <thread>

SpereShape::SpereShape()
{

}

SpereShape::~SpereShape()
{

}

bool SpereShape::Init()
{
	shader = LoadShaders("Shaders/AudioRect.vs","Shaders/AudioRect.fs");
	if(shader>0)
	{
		MVPID = glGetUniformLocation(shader,"MVP");
	}
	return true;
}

void SpereShape::Draw(Visualizer* visualizer)
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
	Model = glm::translate(Model,glm::vec3(0,0,-5));
	glm::mat4 MVP = Projection*View*Model;
	glClearColor(0.3,0.3,0.3,1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,&indices[0]);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}

unsigned int SpereShape::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	// �����ܺͲ�����Ԫ�ظ���
	GenerateNoisySphere(heigthlist,m_SphereRow,m_SphereCol);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

// ʹ�ü򵥵�����������������һ���򵥵�α�������������
// ������滻Ϊ Perlin �������� Simplex ��������ȡ��ƽ����Ч��
float simpleNoise(float x,float y,float z)
{
	return std::sin(x*12.9898f+y*78.233f+z*37.719f)*43758.5453f-std::floor(std::sin(x*12.9898f+y*78.233f+z*37.719f)*43758.5453f);
}
// �������ɺ��������������Ŷ���
void SpereShape::GenerateNoisySphere(const std::vector<float>& heigthlist,int stacks,int slices)
{
	// �������嶥��
	if(heigthlist.empty())return;
	vertices.clear();
	indices.clear();

	auto radius = m_fSphereRadius;
	float sum = std::accumulate(heigthlist.begin(),heigthlist.end(),0.0f);
	radius += sum/heigthlist.size();

	//float noise = (rand()%1000)/1000.0f*0.5f;  // �������һ���Ŷ�ֵ
	//float scale = 1.0f+noise;  // �Ŷ���ĳ߶�����
	//radius*= scale;
	float maxDisturbance = 2;
	for(int i = 0; i<=stacks; ++i)
	{
		float lat = M_PI*(i/(float)stacks-0.5f); // γ��
		for(int j = 0; j<=slices; ++j)
		{
			float lon = 2*M_PI*(j/(float)slices); // ����

			// ������������
			auto x = radius*cos(lat)*cos(lon);
			auto y = radius*cos(lat)*sin(lon);
			auto z = radius*sin(lat);

			vertices.push_back(glm::vec3(x,y,z));
		}
	}
	// ��������
	for(int i = 0; i<stacks; ++i)
	{
		for(int j = 0; j<slices; ++j)
		{
			int first = (i*(slices+1))+j;
			int second = first+slices+1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first+1);

			indices.push_back(second);
			indices.push_back(second+1);
			indices.push_back(first+1);
		}
	}
}