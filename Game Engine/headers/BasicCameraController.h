/*
Author Graham Alexander MacDonald
*/
#ifndef BasicCameraController_h
#define BasicCameraController_h

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Component.h"
#include <vector>

class BasicCameraController :public Camera
{
public:

	BasicCameraController();
	~BasicCameraController();

	void setLookingAtTarget(bool b);
	bool getLookingAtTarget();

	void addGameObjectToTargets(GameObject *go);
	void iterateThroughTargets();

	void moveCamera(std::string strDir);
	void rotateCameraHorizontal(int iDir);
	void rotateCameraVertical(int iDir);

	void rotateObject(int iDir);
	void moveObject(std::string strDir);

	void promptInputForLight();

	int getDir();

	void update();
	void Destroy();

protected:

private:

	bool m_bLookAtTarget;
	char m_cAxis;

	int m_iDir;

	std::vector<GameObject*> m_vTargetGameObjects;
	GameObject* m_goTarget;
	int m_iTargetIndex;

	float m_fAngle;
	float m_fSinX;
	float m_fCosZ;

	float m_fObjectMoveSpeed;
	float m_fObjectRotateSpeed;
};

#endif