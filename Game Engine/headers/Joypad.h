#ifndef JOYPAD_H
#define JOYPAD_H

#include <glm/glm.hpp>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#elif WIN32
#include <SDL.h>
#endif

using glm::vec2;

//http://devcry.heiho.net/2014/07/game-on.html
class Joypad
{
	enum AxisID
	{
		LEFT_AXIS_HORIZONTAL = 0,
		LEFT_AXIS_VERTICAL =1,
		RIGHT_AXIS_VERTICAL = 2,
		RIGHT_AXIS_HORIZONTAL =3,
		LEFT_TRIGGER=4,
		RIGHT_TRIGGER=5,
	};
public:
	Joypad(SDL_GameController * SDLController);
	~Joypad();

	void setButtonUp(short buttonIndex);
	void setButtonDown(short buttonIndex);

	void setAxisValue(short axisID, int value);

	bool isButtonDown(short buttonIndex)
	{
		return m_ButtonsDown[buttonIndex];
	};

	bool isButtonUp(short buttonIndex)
	{
		return m_ButtonsUp[buttonIndex];
	};

	vec2& getLeftThumbstick()
	{
		return m_LeftThumbstick;
	};

	vec2& getRightTumbstick()
	{
		return m_RightThumbstick;
	};

	float getLeftTrigger()
	{
		return m_LeftTrigger;
	};

	float getRightTrigger()
	{
		return m_RightTrigger;
	};

public:
	const static int DeadzoneNeg = -3200;
	const static int DeadzonePos = 3200;
	const static int MAX_NO_OF_BUTTONS = 15;

private:
	//This assumes we are using a 360 Joypad
	bool m_ButtonsDown[MAX_NO_OF_BUTTONS];
	bool m_ButtonsUp[MAX_NO_OF_BUTTONS];
	SDL_GameController *m_SDLController;

	vec2 m_LeftThumbstick;
	vec2 m_RightThumbstick;
	float m_LeftTrigger;
	float m_RightTrigger;

	bool m_InvertAxis;
};

#endif