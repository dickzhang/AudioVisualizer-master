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

float dot(const int* g,float x,float y,float z)
{
	return g[0]*x+g[1]*y+g[2]*z;
}

float SimplexNoise(float xin,float yin,float zin)
{
	const int grad3[12][3] =
	{
		{ 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
		{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
		{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 }
	};

	const int perm[512] =
	{
		151,160,137,91,90,15,
		// [... truncated for brevity ... fill with full permutation table ...]
		151,160,137,91,90,15
	};

	float n0,n1,n2,n3; // Noise contributions from the four corners
	// Skewing and unskewing factors for 3D
	static const float F3 = 1.0f/3.0f;
	static const float G3 = 1.0f/6.0f;

	float s = (xin+yin+zin)*F3;
	int i = floor(xin+s);
	int j = floor(yin+s);
	int k = floor(zin+s);

	float t = (i+j+k)*G3;
	float X0 = i-t;
	float Y0 = j-t;
	float Z0 = k-t;
	float x0 = xin-X0;
	float y0 = yin-Y0;
	float z0 = zin-Z0;

	int i1,j1,k1;
	int i2,j2,k2;

	if(x0>=y0)
	{
		if(y0>=z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
		else if(x0>=z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
		}
		else
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
		}
	}
	else
	{
		if(y0<z0)
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
		}
		else if(x0<z0)
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
		}
		else
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
	}

	float x1 = x0-i1+G3;
	float y1 = y0-j1+G3;
	float z1 = z0-k1+G3;
	float x2 = x0-i2+2.0f*G3;
	float y2 = y0-j2+2.0f*G3;
	float z2 = z0-k2+2.0f*G3;
	float x3 = x0-1.0f+3.0f*G3;
	float y3 = y0-1.0f+3.0f*G3;
	float z3 = z0-1.0f+3.0f*G3;

	int ii = i&255;
	int jj = j&255;
	int kk = k&255;

	//int gi0 = perm[ii+perm[jj+perm[kk]]]%12;
	//int gi1 = perm[ii+i1+perm[jj+j1+perm[kk+k1]]]%12;
	//int gi2 = perm[ii+i2+perm[jj+j2+perm[kk+k2]]]%12;
	//int gi3 = perm[ii+1+perm[jj+1+perm[kk+1]]]%12;

	int gi0 = 0;
	int gi1 = 0;
	int gi2 = 0;
	int gi3 = 0;

	float t0 = 0.6f-x0*x0-y0*y0-z0*z0;
	if(t0<0) n0 = 0.0f;
	else
	{
		t0 *= t0;
		n0 = t0*t0*dot(grad3[gi0],x0,y0,z0);
	}

	float t1 = 0.6f-x1*x1-y1*y1-z1*z1;
	if(t1<0) n1 = 0.0f;
	else
	{
		t1 *= t1;
		n1 = t1*t1*dot(grad3[gi1],x1,y1,z1);
	}

	float t2 = 0.6f-x2*x2-y2*y2-z2*z2;
	if(t2<0) n2 = 0.0f;
	else
	{
		t2 *= t2;
		n2 = t2*t2*dot(grad3[gi2],x2,y2,z2);
	}

	float t3 = 0.6f-x3*x3-y3*y3-z3*z3;
	if(t3<0) n3 = 0.0f;
	else
	{
		t3 *= t3;
		n3 = t3*t3*dot(grad3[gi3],x3,y3,z3);
	}
	return (32.0f*(n0+n1+n2+n3)+1)/2.0; // Final noise result in range [0,1]
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
	auto qz = sum/heigthlist.size();
	radius += qz;
	float noise = (rand()%1000)/1000.0f*0.5f;  // 随机生成一个扰动值
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
			auto offset = SimplexNoise(x,y,z)*qz*5.0;
			if(offset<1)
			{
				offset = 1;
			}
			vertices.push_back(glm::vec3(x*offset,y*offset,z*offset));
			vertices.push_back(glm::vec4(noise,noise,0.6,1));
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