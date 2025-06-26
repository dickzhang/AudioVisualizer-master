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
#include "stb_image.h"
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

	rectshader = LoadShaders("Shaders/AudioRect.vs","Shaders/AudioRect.fs");
	if(rectshader>0)
	{
		rectMVPID = glGetUniformLocation(rectshader,"MVP");
	}

	//woodTexture = loadTexture("Resources/textures/concreteTexture.png",true);
	woodTexture = loadTexture("Resources/textures/snow.jpg",true);
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
	Model = glm::translate(Model,glm::vec3(0,2,-10));
	glm::mat4 MVP = Projection*View*Model;
	glClearColor(0.3,0.3,0.3,1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
	//glUniform1f(uNoiseScaleID,1.2);
	glUniform1f(uNoiseScaleID,qz*2);
	/*if(qz*qz<0.1)
	{
		qz = 0.1;
	}*/
	glUniform1f(uNoiseStrengthID,qz);
	//std::cout<<"bloom: "<<qz<<std::endl;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,woodTexture);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,&indices[0]);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
	DrawRect(visualizer);
}

void NoiseSpereBall::DrawRect(Visualizer* visualizer)
{
	auto heightlist = visualizer->GetHeightList(m_Framecount%m_TotalNum);
	int average = 8;
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

	auto vao = GenRectVAO(templist);
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f),1280.0f/720.0f,0.1f,1000.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0,0,0),
		glm::vec3(0,0,-1),
		glm::vec3(0,1,0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::translate(Model,glm::vec3(-5,-4,-10));
	glm::mat4 MVP = Projection*View*Model;
	glClearColor(0.3,0.3,0.3,1.0);
	glUseProgram(shader);
	glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,heightlist.size()*6);
	glDeleteVertexArrays(1,&vao);
	m_Framecount++;
}
unsigned int NoiseSpereBall::GenRectVAO(const std::vector<float>& heigthlist)
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	std::vector<glm::vec3> vertices = GetRectVetexData(heigthlist);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*vertices.size(),&vertices[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
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

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)(9*sizeof(float)));
	glEnableVertexAttribArray(3);
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
	double row_uv = 1.0/(stacks-1);
	double col_uv = 1.0/slices;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0,1);
	float color = dist(gen); // 0 到 1 之间的随机数
	for(int i = 0; i<=stacks; ++i)
	{
		for(int j = 0; j<=slices; ++j)
		{
			float u = j*col_uv;
			float v = i*row_uv;
			// 计算球面坐标
			auto x = radius*sin(i*row_delta)*cos(j*col_delta);
			auto y = radius*cos(i*row_delta);
			auto z = radius*sin(i*row_delta)*sin(j*col_delta);

			vertices.push_back(glm::vec3(x,y,z));
			vertices.push_back(glm::vec3(color,color,0.8));
			auto normal = glm::normalize(glm::vec3(x,y,z));
			vertices.push_back(normal);
			u=u>1.0 ? 1.0 : u;
			v = v>1.0 ? 0.0 : 1.0-v;
			vertices.push_back(glm::vec3(u,v,0.0));
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

std::vector<glm::vec3> NoiseSpereBall::GetRectVetexData(const std::vector<float>& heigthlist)
{
	std::vector<glm::vec3> vertexdata;
	if(heigthlist.empty())return vertexdata;
	float Hfactor = 3.0;
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

unsigned int NoiseSpereBall::loadTexture(char const* path,bool gammaCorrection)
{
	unsigned int textureID;
	glGenTextures(1,&textureID);

	int width,height,nrComponents;
	unsigned char* data = stbi_load(path,&width,&height,&nrComponents,0);
	if(data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if(nrComponents==1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if(nrComponents==3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if(nrComponents==4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D,textureID);
		glTexImage2D(GL_TEXTURE_2D,0,internalFormat,width,height,0,dataFormat,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout<<"Texture failed to load at path: "<<path<<std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
