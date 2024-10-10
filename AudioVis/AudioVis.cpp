#include "AudioVis.h"
#include "AudioObject.h"
#include "Visualizer.h"

using namespace std;

int main(int argc, char* argv[])
{
	string wavPath = "FeelNoWays.wav";
	AudioObject audio("Resources/" + wavPath, BUFFER_SIZE);
	if (audio.Init())
	{
		audio.PlaySound();
	}
	else
	{
		cout << "Error: Could not read audio file" << endl;
		return 0;
	}

	// Make the OpenGL renderer
	Visualizer visualizer(1280, 720);
	if (!visualizer.Init())
	{
		cout << "Error opening OpenGL renderer" << endl;
		return 0;
	}
	// Just play audio if not debugging
	while (audio.IsPlaying())
	{
		audio.Update();
		visualizer.Update(audio);
	}
	return 0;
}
