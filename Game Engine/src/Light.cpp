#include "Light.h"
#include "GameObject.h"

Light::Light()
{
	m_DiffuseColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpecularColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightDirection = vec3(1.0f, 0.1f, 1.0f);
	m_Type = "Light";
	m_bGettingBrighter = true;
}

Light::~Light()
{

}

vec4& Light::getSpecularColour()
{
	return m_SpecularColour;
}

void Light::setSpecularColour(float r, float g, float b, float a)
{
	m_SpecularColour = vec4(r, g, b, a);
}

vec4& Light::getDiffuseColour()
{
	return m_DiffuseColour;
}

void Light::setDiffuseColour(float r, float g, float b, float a)
{
	m_DiffuseColour = vec4(r, g, b, a);
}

vec3& Light::getDirection()
{
	return m_LightDirection;
}

void Light::setDirection(float x, float y, float z)
{
	m_LightDirection = vec3(x, y, z);
}

void Light::update()
{
	std::string strName = m_Parent->getName();

	if (strName == "armordrecon")
	{
		if (m_bGettingBrighter)
		{
			if (m_LightDirection.y <= 1.0f) m_LightDirection.y += 0.0001f;
			else m_bGettingBrighter = false;
		}
		else
		{
			if (m_LightDirection.y > 0.001f) m_LightDirection.y -= 0.0001f;
			else m_bGettingBrighter = true;
		}
	}
}
