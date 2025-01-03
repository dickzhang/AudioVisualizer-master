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

class AudioCircle:public DrawBase
{
public:
	AudioCircle();

	virtual~AudioCircle();

	virtual bool Init()override;

	virtual void Draw(Visualizer* visualizer)override;
private:
	unsigned int GenVAO(const std::vector<float>& heigthlist);
	void GetVetexData(const std::vector<float>& heigthlist);
private:
	GLuint shader;
	GLuint MVPID;
	std::vector<glm::vec3> m_Vertexdata;
	std::vector <int> m_Indices;
};

