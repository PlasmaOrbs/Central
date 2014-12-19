//
//  FPSCameraController.h
//  GP2BaseCode
//
//  Created by Brian on 12/12/2014.
//  Copyright (c) 2014 Glasgow Caledonian University. All rights reserved.
//
//  Adapted by Graham Alexander MacDonald

#ifndef FPSCameraController_h
#define FPSCameraController_h

#include "Component.h"
#include <glm/glm.hpp>
using glm::vec3;

#include <vector>

class Camera;

class FPSCameraController:public Component
{
public:

    FPSCameraController();
    ~FPSCameraController();
    
    void update();
    
    void setCamera(Camera * camCam);
    void setSpeed(float fSpeed);

	void addGameObjectToTargets(GameObject *go);

	void LookAtTarget(int iIndex);

private:

	bool m_bViewLocked;
	int m_iIndex;

    float m_fForwardSpeed;
    float m_fStrafeSpeed;
    float m_fLookSpeed;
    Camera * m_camAttachedCamera;
	std::vector<GameObject*> m_vLookAtTargets;
	vec3 m_v3LastLookedPosition;
};


#endif
