//
//  Keyboard.h
//  GP2BaseCode
//
//  Created by Brian on 23/10/2014.
//  Copyright (c) 2014 Glasgow Caledonian University. All rights reserved.
//

#ifndef Keyboard_h
#define Keyboard_h

class Keyboard
{
public:
    Keyboard();
    ~Keyboard();
    
    void setKeyDown(short keyCode);
    void setKeyUp(short keyCode);
    
    bool isKeyDown(short keyCode);
    bool isKeyUp(short keyCode);
    
    void update();
private:
    const static short MAX_NO_KEYS=256;
    bool keysDown[MAX_NO_KEYS];
    bool keysUp[MAX_NO_KEYS];
};


#endif
