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

class AudioObject;
class Visualizer;
// 球体顶点结构体

class NoiseSpereBall :public DrawBase
{
public:
	struct Vertex
	{
		float x,y,z;
		float nx,ny,nz;  // 法线
	};
public:

	NoiseSpereBall();

	virtual~NoiseSpereBall();

	virtual bool Init()override;

	virtual void Draw(Visualizer* visualizer)override;

	void DrawRect(Visualizer* visualizer);
private:
	unsigned int GenVAO(const std::vector<float>& heigthlist);
	unsigned int GenRectVAO(const std::vector<float>& heigthlist);
	void GenerateNoisySphere(const std::vector<float>& heigthlist,int stacks,int slices);
	std::vector<glm::vec3> GetRectVetexData(const std::vector<float>& heigthlist);
private:
	GLuint shader;
	GLuint rectshader;
	GLuint MVPID;
	GLuint rectMVPID;
	GLuint uNoiseScaleID;
	GLuint uNoiseStrengthID;
	float m_fSphereRadius = 1;
	int m_SphereRow = 61;
	int m_SphereCol = 60;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
};
