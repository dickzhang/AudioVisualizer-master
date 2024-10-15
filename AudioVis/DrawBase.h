#pragma once

class AudioObject;
class Visualizer;

class DrawBase
{
public:
	DrawBase();
	virtual ~DrawBase();
	virtual bool Init()=0;
	virtual void Draw(const AudioObject& audioObject, const Visualizer& visualizer)=0;
};

