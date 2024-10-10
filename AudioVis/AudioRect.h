#pragma once

#define _USE_MATH_DEFINES
#ifndef GLFW_DLL
#define GLFW_DLL
#endif

#include "AudioVis.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	
#include "glm/gtx/transform.hpp"	

class AudioObject;
class Visualizer;

class AudioRect
{
public:

	AudioRect();

	~AudioRect();

	bool Init();

	void Draw(const AudioObject& audioObject, const Visualizer& visualizer);

private:
	unsigned int GenVAO(const std::vector<float>& heigthlist);
	std::vector<glm::vec3> GetVetexData(const std::vector<float>& heigthlist);
private:
	GLuint shader;
	GLuint MVPID;
};

