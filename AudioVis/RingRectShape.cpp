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
	auto particlevao = GenParticleVAO();
	auto vao = GenVAO(templist);
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

	glBindVertexArray(particlevao);
	glDrawArrays(GL_TRIANGLES,0,m_ParticleVertex.size()*6);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,heightlist.size()*6);

	glDeleteVertexArrays(1,&particlevao);
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

unsigned int RingRectShape::GenParticleVAO()
{
	unsigned int VBO,VAO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glBindVertexArray(VAO);

	GetParticleVertexData();
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*m_ParticleVertex.size(),&m_ParticleVertex[0],GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	return VAO;
}
void RingRectShape::GetVetexData(const std::vector<float>& heigthlist)
{
	m_Vertexdata.clear();
	int num = heigthlist.size()*0.8;
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

void RingRectShape::GetParticleVertexData()
{
	if(!m_ParticleInfoList.empty())
	{
		int index = 0;
		m_ParticleVertex.clear();
		glm::vec3 color = { 254.0/255.0,164/255.0,67/255.0 };
		for(index;index<m_ParticleInfoList.size(); index)
		{
			auto info = m_ParticleInfoList[index];
			glm::vec3 pos = info.center+info.moveDir*info.speed;
			auto dis = glm::length(pos);
			if(dis<6)
			{
				m_ParticleInfoList[index].center = pos;
				auto pos0 = glm::vec3(pos.x-info.sidelen,pos.y,pos.z);
				auto pos1 = glm::vec3(pos.x+info.sidelen,pos.y,pos.z);
				auto pos2 = glm::vec3(pos.x,pos.y+info.sidelen,pos.z);
				glm::mat4 Model = glm::mat4(1.0f);
				Model = glm::rotate(Model,glm::radians(info.rotate.x),glm::vec3(0.0,0.0,1.0));
				auto pos00 = glm::vec4(pos0,1.0)*Model;
				auto pos11 = glm::vec4(pos1,1.0)*Model;
				auto pos22 = glm::vec4(pos2,1.0)*Model;

				m_ParticleVertex.push_back(pos00);
				m_ParticleVertex.push_back(color);
				m_ParticleVertex.push_back(pos11);
				m_ParticleVertex.push_back(color);
				m_ParticleVertex.push_back(pos22);
				m_ParticleVertex.push_back(color);
				index++;
			}
			else
			{
				m_ParticleInfoList.erase(m_ParticleInfoList.begin()+index);
			}
		}
	}
	if(m_ParticleInfoList.size()>20)
	{
		return;
	}
	int segmentNum = 2;
	float row_delta = -glm::radians(360.0f)/segmentNum;
	glm::vec3 color = { 254.0/255.0,164/255.0,67/255.0 };

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0,1);
	auto startRadian = glm::radians(dis(gen)*360);

	for(int i = 0; i<segmentNum; i++)
	{
		float sidelen = dis(gen)*0.5+0.2;
		float m_fRadius = 2.0;
		auto cosV = glm::cos(startRadian+i*row_delta);
		auto sinV = glm::sin(startRadian+i*row_delta);
		glm::vec3 pos = glm::vec3(m_fRadius*cosV,m_fRadius*sinV,0.0f);
		ParticleInfo info;
		info.center = pos;
		info.moveDir = glm::normalize(pos);
		info.rotate = GenerateRandomRotate(dis,gen);
		info.sidelen = sidelen;
		info.speed = dis(gen)*0.03+0.01;
		m_ParticleInfoList.push_back(info);

		auto pos0 = glm::vec3(pos.x-sidelen,pos.y,pos.z);
		auto pos1 = glm::vec3(pos.x+sidelen,pos.y,pos.z);
		auto pos2 = glm::vec3(pos.x,pos.y+sidelen,pos.z);
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::rotate(Model,glm::radians(info.rotate.x),glm::vec3(0.0,0.0,1.0));
		auto pos00 = glm::vec4(pos0,1.0)*Model;
		auto pos11 = glm::vec4(pos1,1.0)*Model;
		auto pos22 = glm::vec4(pos2,1.0)*Model;
		m_ParticleVertex.push_back(pos00);
		m_ParticleVertex.push_back(color);
		m_ParticleVertex.push_back(pos11);
		m_ParticleVertex.push_back(color);
		m_ParticleVertex.push_back(pos22);
		m_ParticleVertex.push_back(color);
	}
}

glm::vec3 RingRectShape::GenerateRandomRotate(std::uniform_real_distribution<>& dis,std::mt19937& gen)
{
	glm::vec3 rotate = glm::vec3(0.0);
	for(int i = 0; i<3; i++)
	{
		float min = m_Rotate_min_bounds[i];
		float max = m_Rotate_max_bounds[i];
		rotate[i] = dis(gen)*(max-min)+min;
	}
	return rotate;
}

glm::vec3 RingRectShape::GenerateRandomScale(std::uniform_real_distribution<>& dis,std::mt19937& gen)
{
	glm::vec3 scale = glm::vec3(0.0);
	for(int i = 0; i<2; i++)
	{
		float min = m_Scale_min_bounds[i];
		float max = m_Scale_max_bounds[i];
		scale[i] = dis(gen)*(max-min)+min;
	}
	return scale;
}
