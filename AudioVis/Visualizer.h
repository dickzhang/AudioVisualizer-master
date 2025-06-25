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

//0 ���𶯵ľ��β���ʾ��
//1 ��Բ�β���ʾ��
//2 ��Բ���в���ʾ��
//3 ����Ӱ�ľ���ʾ��
//4 ����Ӱ����Ƭ��ʾ��
//5 Բ��˫�򲨶�ʾ��
//6 �������岨��Ч��
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