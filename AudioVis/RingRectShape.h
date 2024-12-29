#pragma once

#define _USE_MATH_DEFINES

#ifndef GLFW_DLL
#define GLFW_DLL
#endif

#include "AudioVis.h"
#include "DrawBase.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	
#include "glm/gtx/transform.hpp"	
#include <random>

class AudioObject;
class Visualizer;

struct ParticleInfo
{
	glm::vec3 center;
	glm::vec3 moveDir;
	float speed = 0.01;
	float sidelen=0.3;
	glm::vec3 rotate;
};
class RingRectShape :public DrawBase
{
public:
	RingRectShape();

	virtual ~RingRectShape();

	virtual bool Init()override;

	virtual void Draw(Visualizer* visualizer)override;
private:
	unsigned int GenVAO(const std::vector<float>& heigthlist);
	void GetVetexData(const std::vector<float>& heigthlist);
	unsigned int GenParticleVAO();
	void GetParticleVertexData();
	glm::vec3 GenerateRandomRotate(std::uniform_real_distribution<>& dis,std::mt19937& gen);
	glm::vec3 GenerateRandomScale(std::uniform_real_distribution<>& dis,std::mt19937& gen);
private:
	GLuint shader;
	GLuint MVPID;
	std::vector<glm::vec3> m_Vertexdata;
	std::vector<glm::vec3> m_ParticleVertex;
	std::vector<ParticleInfo> m_ParticleInfoList;
	glm::vec3 m_Rotate_min_bounds = glm::vec3(0,0,0);
	glm::vec3 m_Rotate_max_bounds = glm::vec3(360,360,360);

	glm::vec3 m_Scale_min_bounds = glm::vec3(0.5,0.5,1);
	glm::vec3 m_Scale_max_bounds = glm::vec3(3,3,1);
};

