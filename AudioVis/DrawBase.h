#pragma once

class AudioObject;
class Visualizer;

class DrawBase
{
public:
	DrawBase();
	virtual ~DrawBase();
	virtual bool Init()=0;
	virtual void Draw(const AudioObject& audioObject,const Visualizer& visualizer)
	{
	}
	virtual void Draw(Visualizer* visualizer)=0;
protected:
	int m_Framecount;
	int m_TotalNum = 636;
};

