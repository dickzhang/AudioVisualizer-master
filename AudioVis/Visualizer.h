#pragma once

#define _USE_MATH_DEFINES

#ifndef GLFW_DLL
#define GLFW_DLL
#endif

#include "AudioVis.h"
#include "Object3D.h"
#include "AudioRect.h"
#include "AudioCircle.h"
#include "AudioRing.h"
#include "DrawBase.h"
#include <stdio.h>
#include <chrono>	
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

using namespace std;
using namespace glm;
using namespace chrono;
class AudioObject;


//0 是动感模型示例
//1 是震动的矩形波浪示例
//2 是圆形波浪示例
//3 是圆环行波浪示例
#define DEMOTYPE 3

class Visualizer
{
public:
	Visualizer(int width, int height);
	~Visualizer();
	bool Init();
	void Update(const AudioObject& audioObject);
	const double& GetDeltaTime() const
	{
		return deltaTime;
	}
private:
	bool InitWindow();
	void InitVAO();
	void Teardown();
	DrawBase* GetDrawObject();
private:

	GLFWwindow* window;
	GLuint vertexArrayID;
	int windowWidth{ 0 };
	int windowHeight{ 0 };

	Object3D m_Object3D;
	AudioRect m_AudioRect;
	AudioCircle m_AudioCircle;
	AudioRing m_AudioRing;
	DrawBase* m_DrawBase;
	double						deltaTime{ 0 };
	time_point<steady_clock>	lastTimeStamp;
};