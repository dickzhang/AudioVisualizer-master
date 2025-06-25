#pragma once

#define _USE_MATH_DEFINES

#ifndef GLFW_DLL
#define GLFW_DLL
#endif

#include "AudioVis.h"
#include "AudioRect.h"
#include "AudioCircle.h"
#include "AudioRing.h"
#include "RectShape.h"
#include "RingRectShape.h"
#include "DrawBase.h"
#include "LineAreaShape.h"
#include "NoiseSpereBall.h"
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

//0 是震动的矩形波浪示例
//1 是圆形波浪示例
//2 是圆环行波浪示例
//3 带阴影的矩形示例
//4 带阴影的面片形示例
//5 圆环双向波动示例
//6 噪声球体波动效果
#define DEMOTYPE 6

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

	RectShape m_RectShape;
	AudioRect m_AudioRect;
	LineAreaShape m_LineAreaShape;
	AudioCircle m_AudioCircle;
	AudioRing m_AudioRing;
	RingRectShape m_RingRectShape;
	NoiseSpereBall m_NoiseSpereBall;
	DrawBase* m_DrawBase;
	double						deltaTime{ 0 };
	time_point<steady_clock>	lastTimeStamp;
	json m_JsonData;
	vector<float> m_AudioData;
};