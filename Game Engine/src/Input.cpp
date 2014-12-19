//
//  Input.cpp
//  GP2BaseCode
//
//  Created by Brian on 23/10/2014.
//  Copyright (c) 2014 Glasgow Caledonian University. All rights reserved.
//

#include "Input.h"


Input::Input()
{
    m_Mouse=NULL;
    m_Keyboard=NULL;
}

Input::~Input()
{
	
}

void Input::destroy()
{
	auto iter = m_AttachedJoypads.begin();
	while (iter != m_AttachedJoypads.end())
	{
		if ((*iter))
		{
			delete (*iter);
			iter = m_AttachedJoypads.erase(iter);
		}
		else
			iter++;
	}
	m_AttachedJoypads.clear();
    
    if (m_Mouse)
    {
        delete m_Mouse;
        m_Mouse=NULL;
    }
    if (m_Keyboard)
    {
        delete m_Keyboard;
        m_Keyboard=NULL;
    }
}

void Input::update()
{
    m_Keyboard->update();
}

bool Input::init(const std::string& inputDBFilename)
{
    m_Keyboard=new Keyboard();
    m_Mouse=new Mouse();
	SDL_GameControllerAddMappingsFromFile(inputDBFilename.c_str());
	int numOfJoypads = SDL_NumJoysticks();
	for (int i = 0; i < numOfJoypads; i++)
	{
		//Is the joypad able to use the new controller interface
		if (SDL_IsGameController(i))
		{
			SDL_GameController *controller = SDL_GameControllerOpen(i);
			Joypad * joypad = new Joypad(controller);
			m_AttachedJoypads.push_back(joypad);
		}
	}

    return true;
}