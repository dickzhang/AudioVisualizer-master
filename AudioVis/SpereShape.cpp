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
	// 计算总和并除以元素个数
	GenerateNoisySphere(heigthlist,m_SphereRow,m_SphereCol);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

// 使用简单的噪声函数（这里是一个简单的伪随机生成噪声）
// 你可以替换为 Perlin 噪声或者 Simplex 噪声来获取更平滑的效果
float simpleNoise(float x,float y,float z)
{
	return std::sin(x*12.9898f+y*78.233f+z*37.719f)*43758.5453f-std::floor(std::sin(x*12.9898f+y*78.233f+z*37.719f)*43758.5453f);
}
// 球体生成函数（包括噪声扰动）
void SpereShape::GenerateNoisySphere(const std::vector<float>& heigthlist,int stacks,int slices)
{
	// 生成球体顶点
	if(heigthlist.empty())return;
	vertices.clear();
	indices.clear();

	auto radius = m_fSphereRadius;
	float sum = std::accumulate(heigthlist.begin(),heigthlist.end(),0.0f);
	radius += sum/heigthlist.size();

	//float noise = (rand()%1000)/1000.0f*0.5f;  // 随机生成一个扰动值
	//float scale = 1.0f+noise;  // 扰动后的尺度因子
	//radius*= scale;
	float maxDisturbance = 2;
	for(int i = 0; i<=stacks; ++i)
	{
		float lat = M_PI*(i/(float)stacks-0.5f); // 纬度
		for(int j = 0; j<=slices; ++j)
		{
			float lon = 2*M_PI*(j/(float)slices); // 经度

			// 计算球面坐标
			auto x = radius*cos(lat)*cos(lon);
			auto y = radius*cos(lat)*sin(lon);
			auto z = radius*sin(lat);

			vertices.push_back(glm::vec3(x,y,z));
		}
	}
	// 生成索引
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