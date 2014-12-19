//
//  Mouse.h
//  GP2BaseCode
//
//  Created by Brian on 23/10/2014.
//  Copyright (c) 2014 Glasgow Caledonian University. All rights reserved.
//

#ifndef Mouse_h
#define Mouse_h

class Mouse
{
public:
    Mouse();
    ~Mouse();
    
    void update();
    
    void setMousePosition(int absX,int absY,int relX,int relY);
    
    void setMouseButtonDown(int buttonIndex);
    void setMouseButtonUp(int buttonIndex);
    
    bool isButtonDown(int buttonIndex);
    bool isButtonUp(int buttonIndex);
    
    int getRelativeMouseX()
    {
        return m_RelMouseX;
    };
    
    int getRelativeMouseY()
    {
        return m_RelMouseY;
    };
    
private:
    static const int MAX_NO_OF_BUTTONS=3;
    int m_AbsMouseX;
    int m_AbsMouseY;
    int m_RelMouseX;
    int m_RelMouseY;
    bool m_MouseButtonsDown[MAX_NO_OF_BUTTONS];
    bool m_MouseButtonsUp[MAX_NO_OF_BUTTONS];
};

#endif
