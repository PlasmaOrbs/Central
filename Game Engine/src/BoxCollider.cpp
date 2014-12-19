// Author Graham Alexander MacDonald and Adrian Lis
#include <iostream>
#include "GameObject.h"
#include "Transform.h"

#include <glm/glm.hpp>
using glm::vec3;

#include "BoxCollider.h"

BoxCollider::BoxCollider()
{
	m_Active = true;
	m_Type = "BoxCollider";
}

BoxCollider::~BoxCollider()
{

}

float BoxCollider::getLength()
{
	return m_fLength;
}

float BoxCollider::getWidth()
{
	return m_fWidth;
}

float BoxCollider::getHeight()
{
	return m_fHeight;
}

void BoxCollider::setLength(float x)
{
	m_fLength = x;
}

void BoxCollider::setHeight(float y)
{
	m_fHeight = y;
}

void BoxCollider::setWidth(float z)
{
	m_fWidth = z;
}

void BoxCollider::setCentre(vec3 v3Centre)
{
	m_v3Centre = v3Centre;
}

bool BoxCollider::checkForCollision(vec3 pos)
{
	//vec3 v3Centre = m_Parent->getTransform()->getPosition();

	if (pos.x > (m_v3Centre.x - (m_fLength / 2)) && pos.x < (m_v3Centre.x + (m_fLength / 2)));
	else return false;

	if (pos.y >(m_v3Centre.y - (m_fHeight / 2)) && pos.y < (m_v3Centre.y + (m_fHeight / 2)));
	else return false;

	if (pos.z >(m_v3Centre.z - (m_fWidth / 2)) && pos.z < (m_v3Centre.z + (m_fWidth / 2)));
	else return false;

	return true;
}



