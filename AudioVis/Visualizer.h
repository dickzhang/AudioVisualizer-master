#pragma once

#define _USE_MATH_DEFINES

#ifndef GLFW_DLL
#define GLFW_DLL
#endif

#include "AudioVis.h"
#include "AudioRect.h"
#include "AudioCircle.h"
#include "AudioRing.h"
#include "DrawBase.h"
#include <stdio.h>
#include <chrono>	
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "json.hpp"

using namespace std;
using namespace glm;
using namespace chrono;
class AudioObject;
using json = nlohmann::json;

//0 ���𶯵ľ��β���ʾ��
//1 ��Բ�β���ʾ��
//2 ��Բ���в���ʾ��
#define DEMOTYPE 2

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
	vector<float> GetHeightList(int index);

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

	AudioRect m_AudioRect;
	AudioCircle m_AudioCircle;
	AudioRing m_AudioRing;
	DrawBase* m_DrawBase;
	double						deltaTime{ 0 };
	time_point<steady_clock>	lastTimeStamp;
	json m_JsonData;
};