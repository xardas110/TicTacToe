#pragma once
class KeyEvent
{
public:
	enum class KeyState
	{
		Released, Pressed
	};
	int key, scancode, mods;
	KeyState state;
	KeyEvent(int key, int scancode, KeyState state, int mods)
		:key(key), scancode(scancode), state(state), mods(mods)
	{};
};

class UpdateEvent
{
public: 
	UpdateEvent(double deltatime, double totaltime, uint64_t framenr)
	: deltaTime(deltatime), totalTime(totaltime), frameNr(framenr)
{}
	double deltaTime;
	double totalTime;
	uint64_t frameNr;
};