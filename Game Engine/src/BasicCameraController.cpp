/*
Author Graham Alexander MacDonald
*/

#include "BasicCameraController.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include  <math.h>
#include <iostream>
#include "Timer.h"
#include "Input.h"
#include "Light.h"

#include <glm/glm.hpp>
using glm::vec3;

/*
Constructor
*/
BasicCameraController::BasicCameraController()
{
	m_Type = "BasicCameraController";
	m_fAngle = 0.0f;
	m_fSinX = 0.0f,
	m_fCosZ = -1.0f;
	m_bLookAtTarget = false;
	m_iTargetIndex = 0;
	m_fObjectMoveSpeed = 1.0f;
	m_fObjectRotateSpeed = 1.0f;
	m_cAxis = 'x';
	m_iDir = 0;
}

/*
Deconstructor
*/
BasicCameraController::~BasicCameraController()
{

}

void BasicCameraController::setLookingAtTarget(bool b)
{
	if (b) std::cout << "\nCamera View: Locked (Press 'TAB' to iterate through GameObjects)" << std::endl;
	if (!b) std::cout << "\nCamera View: Unlocked" << std::endl;
	m_bLookAtTarget = b;
}

bool BasicCameraController::getLookingAtTarget()
{
	return m_bLookAtTarget;
}

void BasicCameraController::addGameObjectToTargets(GameObject *go)
{
	m_vTargetGameObjects.push_back(go);
}

void BasicCameraController::iterateThroughTargets()
{	
	if (m_iTargetIndex < m_vTargetGameObjects.size()-1) m_iTargetIndex++;
	else m_iTargetIndex = 0;

	m_goTarget = m_vTargetGameObjects[m_iTargetIndex];
	std::cout << " - Targetted: " << m_vTargetGameObjects[m_iTargetIndex]->getName() << std::endl;
}

/*
Moves camera forward, backward, up and down using
a string
*/
void BasicCameraController::moveCamera(std::string strDir)
{
	float fX = m_Parent->getTransform()->getPosition().x;
	float fY = m_Parent->getTransform()->getPosition().y;
	float fZ = m_Parent->getTransform()->getPosition().z;

	m_iDir = 0; //standing still
	
	if (strDir == "forward")
	{
		fX += m_fSinX * 0.1f;
		fZ += m_fCosZ * 0.1f;
		m_iDir = 1; //moving forward
	}

	if (strDir == "backward")
	{
		fX -= m_fSinX * 0.1f;
		fZ -= m_fCosZ * 0.1f;
		m_iDir = -1; //moving backward
	}

	if (strDir == "up")
	{
		fY += 0.01f;
	}

	if (strDir == "down")
	{
		fY -= 0.01f;
	}

	m_Parent->getTransform()->setPosition(fX, fY, fZ);
	Camera::m_LookAt = vec3(fX + m_fSinX, fY, fZ + m_fCosZ);
}

/*
Rotates the camera around the Y axis using a direction
*/
void BasicCameraController::rotateCameraHorizontal(int iDir)
{	
	if (!m_bLookAtTarget)
	{
		if (iDir < 0)
		{
			m_fAngle -= 0.1f;
			m_fSinX = sin(m_fAngle);
			m_fCosZ = -cos(m_fAngle);
		}

		if (iDir > 0)
		{
			m_fAngle += 0.1f;
			m_fSinX = sin(m_fAngle);
			m_fCosZ = -cos(m_fAngle);
		}

		vec3 v3Pos = m_Parent->getTransform()->getPosition();
		m_LookAt = vec3(v3Pos.x + m_fSinX,
			v3Pos.y,
			v3Pos.z + m_fCosZ);
	}
}

int BasicCameraController::getDir()
{
	return m_iDir;
}

void BasicCameraController::moveObject(std::string str)
{
	vec3 v3Pos = m_vTargetGameObjects[m_iTargetIndex]->getTransform()->getPosition();

	if (str == "forward")
	{
		float fX = v3Pos.x + m_fObjectMoveSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setPosition(fX, v3Pos.y, v3Pos.z);
		
	}
	if (str == "backward")
	{
		float fX = v3Pos.x - m_fObjectMoveSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setPosition(fX, v3Pos.y, v3Pos.z);
	}
	if (str == "right")
	{
		float fZ = v3Pos.z + m_fObjectMoveSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setPosition(v3Pos.x, v3Pos.y, fZ);
	}
	if (str == "left")
	{
		float fZ = v3Pos.z - m_fObjectMoveSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setPosition(v3Pos.x, v3Pos.y, fZ);
	}
}

void BasicCameraController::rotateObject(int iDir)
{
	vec3 v3Rot = m_vTargetGameObjects[m_iTargetIndex]->getTransform()->getRotation();
	float f;

	if (m_cAxis == 'x')
	{
		if(iDir > 0) f = v3Rot.x + m_fObjectRotateSpeed;
		else f = v3Rot.x - m_fObjectRotateSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setRotation(f, v3Rot.y, v3Rot.z);
	}

	if (m_cAxis == 'y')
	{
		if (iDir > 0) f = v3Rot.y + m_fObjectRotateSpeed;
		else f = v3Rot.y - m_fObjectRotateSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setRotation(v3Rot.x, f, v3Rot.z);
	}

	if (m_cAxis == 'z')
	{
		if (iDir > 0) f = v3Rot.z + m_fObjectRotateSpeed;
		else f = v3Rot.z - m_fObjectRotateSpeed;
		m_vTargetGameObjects[m_iTargetIndex]->getTransform()->setRotation(v3Rot.x, v3Rot.y, f);
	}
}

/*
Memory Clean Up
*/
void BasicCameraController::Destroy()
{
	m_vTargetGameObjects.clear();
}

/* //Errors in accessing variables at run time
void BasicCameraController::promptInputForLight()
{
	float fX, fY, fZ;

	std::cout << "'L' pressed : Please enter an float value between 0 and 1 for \n x, y and z values that adjust light direction ";
	
	std::cout << "x = ";
	std::cin >> fX;

	std::cout << "\ny = ";
	std::cin >> fY;

	std::cout << "\nz = ";
	std::cin >> fZ;

	m_Parent->getLight()->setDirection(fX, fY, fZ);
}
*/

/*
Updates matrices for Projection and view
using parents>transform>position
*/
void BasicCameraController::update()
{	
	//Grab input
	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_w))
	{
		moveCamera("forward");
	}
	else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_s))
	{
		moveCamera("backward");
	}

	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_a))
	{
		rotateCameraHorizontal(-1);
	}
	else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_d))
	{
		rotateCameraHorizontal(1);
	}

	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_CAPSLOCK))
	{
		setLookingAtTarget(!m_bLookAtTarget);
	}

	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_TAB) && m_bLookAtTarget)
	{
		iterateThroughTargets();
	}

	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_l) && m_bLookAtTarget)
	{
		;
		//promptInputForLight();
	}

	if (Input::getInput().getKeyboard()->isKeyDown(SDLK_F1))
	{
		std::cout << "\nCONTROLS: Press F1 for 'help' (command list)" << std::endl;
		std::cout << "\nNAVIGATION:\n" << std::endl;
		std::cout << "[W]: Move camera FORWARD" << std::endl;
		std::cout << "[S]: Move camera BACKWARD" << std::endl;
		std::cout << "[A]: Rotate camera LEFT" << std::endl;
		std::cout << "[D]: Rotate camera RIGHT" << std::endl;
		std::cout << "[SPACE]: Move camera UP" << std::endl;
		std::cout << "[LEFT_SHIFT]: Move camera DOWN\n" << std::endl;
		std::cout << "OBJECT MANIPULATION\n" << std::endl;
		std::cout << "[CAPS_LOCK]: Target scene objects " << std::endl;
		std::cout << "[TAB]: Iterate through scene objects\n" << std::endl;
		std::cout << "...while locked onto an object" << std::endl;
		std::cout << "[Q]: Rotate positively round an object " << std::endl;
		std::cout << "[E]: Rotate negatively round an object\n" << std::endl;
		std::cout << "...while holding " << std::endl;
		std::cout << "[X]: round the x-axis " << std::endl;
		std::cout << "[Y]: round the y-axis " << std::endl;
		std::cout << "[Z]: round the z-axis\n" << std::endl;
		std::cout << "...To move objects" << std::endl;
		std::cout << "[UP_ARROW][DOWN_ARROW][LEFT_ARROW][RIGHT_ARROW]" << std::endl;
	}

	//if locked onto a target
	if (m_bLookAtTarget)
	{
		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_UP)) moveObject("forward");
		else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_DOWN)) moveObject("backward");
		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_LEFT)) moveObject("left");
		else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_RIGHT)) moveObject("right");

		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_x)) m_cAxis = 'x';
		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_y)) m_cAxis = 'y';
		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_z)) m_cAxis = 'z';

		if (Input::getInput().getKeyboard()->isKeyDown(SDLK_q)) rotateObject(1);
		else if (Input::getInput().getKeyboard()->isKeyDown(SDLK_e)) rotateObject(-1);
	}
	
	//get the position from the transform
	vec3 v3CameraPosition = m_Parent->getTransform()->getPosition();

	m_Projection = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
	
	//if looking at target
	if (m_bLookAtTarget)
	{
		vec3 v3LookPosition = m_vTargetGameObjects[m_iTargetIndex]->getTransform()->getPosition();
		m_View = glm::lookAt(v3CameraPosition, v3LookPosition, m_Up);
	}
	else m_View = glm::lookAt(v3CameraPosition, m_LookAt, m_Up); //not looking at target
}