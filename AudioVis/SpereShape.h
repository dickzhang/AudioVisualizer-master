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

class SpereShape :public DrawBase
{
public:
	struct Vertex
	{
		float x,y,z;
		float nx,ny,nz;  // 法线
	};
public:

	SpereShape();

	virtual~SpereShape();

	virtual bool Init()override;

	virtual void Draw(Visualizer* visualizer)override;

private:
	unsigned int GenVAO(const std::vector<float>& heigthlist);
	void GenerateNoisySphere(const std::vector<float>& heigthlist,int stacks,int slices);
private:
	GLuint shader;
	GLuint MVPID;
	float m_fSphereRadius = 1;
	int m_SphereRow = 61;
	int m_SphereCol = 60;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
};
