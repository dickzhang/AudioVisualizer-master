#include "NoiseSpereBall.h"
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

NoiseSpereBall::NoiseSpereBall()
{

}

NoiseSpereBall::~NoiseSpereBall()
{

}

bool NoiseSpereBall::Init()
{
	shader = LoadShaders("Shaders/NoiseBall.vs","Shaders/NoiseBall.fs");
	if(shader>0)
	{
		MVPID = glGetUniformLocation(shader,"MVP");
		uNoiseScaleID = glGetUniformLocation(shader,"uNoiseScale");
		uNoiseStrengthID = glGetUniformLocation(shader,"uNoiseStrength");
	}
	return true;
}

void NoiseSpereBall::Draw(Visualizer* visualizer)
{
	auto heightlist = visualizer->GetHeightList(m_Framecount%m_TotalNum);
	float sum = std::accumulate(heightlist.begin(),heightlist.end(),0.0f);
	auto qz = sum/heightlist.size();
	auto vao = GenVAO(heightlist);
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
	glUniform1f(uNoiseScaleID,1.2);
	glUniform1f(uNoiseStrengthID,qz);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,&indices[0]);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}

unsigned int NoiseSpereBall::GenVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	// 计算总和并除以元素个数
	GenerateNoisySphere(heigthlist,m_SphereRow,m_SphereCol);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	return VAO;
}

// 球体生成函数（包括噪声扰动） 
void NoiseSpereBall::GenerateNoisySphere(const std::vector<float>& heigthlist,int stacks,int slices)
{
	// 生成球体顶点
	if(heigthlist.empty())return;
	vertices.clear();
	indices.clear();

	auto radius = m_fSphereRadius;
	float sum = std::accumulate(heigthlist.begin(),heigthlist.end(),0.0f);
	auto qz = sum/heigthlist.size();
	radius += qz*2;

	double row_delta = M_PI/(stacks-1);
	double col_delta = 2*M_PI/slices;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0,1);
	float color = dist(gen); // 0 到 1 之间的随机数
	for(int i = 0; i<=stacks; ++i)
	{
		for(int j = 0; j<=slices; ++j)
		{
			// 计算球面坐标
			auto x = radius*sin(i*row_delta)*cos(j*col_delta);
			auto y = radius*cos(i*row_delta);
			auto z = radius*sin(i*row_delta)*sin(j*col_delta);

			vertices.push_back(glm::vec3(x,y,z));
			vertices.push_back(glm::vec3(color,color,0.8));
			auto normal = glm::vec3(x,y,z);
			normal = glm::normalize(normal);
			vertices.push_back(normal);
		}
	}
	// 生成索引
	for(int i = 0; i<stacks-1; ++i)
	{
		for(int j = 0; j<slices; ++j)
		{
			// Triangle0
			indices.push_back(i*(slices+1)+j+1);
			indices.push_back((i+1)*(slices+1)+j);
			indices.push_back(i*(slices+1)+j);
			// Triagnle1
			indices.push_back(i*(slices+1)+j+1);
			indices.push_back((i+1)*(slices+1)+(j+1));
			indices.push_back((i+1)*(slices+1)+j);
		}
	}
}