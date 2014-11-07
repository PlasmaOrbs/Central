#version 150

in Vec3 vertexPosition;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * Vec4(vertexPosition, 1.0);
}