//by Graham Alexander MacDonald and Adrian Lis
#ifndef BoxCollider_h
#define BoxCollider_h

#include "Component.h"

#include <glm/glm.hpp>
using glm::vec3;

class BoxCollider : public Component
{
public:

	BoxCollider();
	~BoxCollider();

	bool checkForCollision(vec3 pos);

	float getLength();
	float getHeight();
	float getWidth();

	void setLength(float x);
	void setHeight(float y);
	void setWidth(float z);

	void setCentre(vec3 v3Centre);

private:

	float m_fLength;
	float m_fWidth;
	float m_fHeight;

	vec3 m_v3Centre;
};

#endif

