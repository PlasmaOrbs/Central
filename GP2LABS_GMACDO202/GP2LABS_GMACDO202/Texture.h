#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SDL_TTF.h> //for fonts

#ifdef WIN32
#include <SDL_opengl.h>
#include <SDL_image.h>
#elif __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2_image/SDL_image.h>
#endif

using namespace std;

#include <iostream>
#include <string>

/* Takes a string that represent the path to the filename
to load and return a GLuint which represents the ID of 
the loaded OpenGL Texture. */
GLuint loadTextureFromFile(const string& strFilename);

/* Takes a string that represents the path to the filename
to load and return a GLuint which represents the ID of
the loaded OpenGL Font. */
GLuint	loadTextureFromFont(const string& strFontFilename,
	int iPointSize,
	const string& strText);

/* Convert surface to OpenGL Texture */
GLuint convertSDLSurfaceToGLTexture(SDL_Surface *sdl_surface);

#endif