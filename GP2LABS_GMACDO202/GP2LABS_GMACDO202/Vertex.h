/*
A header guard ensures that the code in the file between
#define and #endif will only be compiled once
*/
#ifndef Vertex_h
#define Vertex_h

#include <glm/glm.hpp>
using glm::vec4;
using glm::vec3;
using glm::vec2;

struct Vertex
{
	vec3 position;
	vec2 texCoords;
	vec4 colour;
};

#endif