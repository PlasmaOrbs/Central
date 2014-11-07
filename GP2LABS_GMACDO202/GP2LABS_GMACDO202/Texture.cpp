#include "Texture.h"

using namespace std;

/* Using SDL Image to load texture from file */
GLuint loadTextureFromFile(const string& filename)
{
	GLuint textureID = 0;

	#pragma region Using SDL Image to load Texture
	SDL_Surface	*sdl_surfaceTexture = IMG_Load(filename.c_str());

	if (!sdl_surfaceTexture)
	{
		cout << "Can't Load image " << filename << " " << IMG_GetError();
		return textureID;
	}
	#pragma endregion


	#pragma region Convert to OpenGL texture
	//number of bytes per pixel
	GLint gliNumOfColors = sdl_surfaceTexture->format->BytesPerPixel;
	GLenum glenumTextureFormat = GL_RGB;

	if (gliNumOfColors == 4) //if 4 contains rgba channels
	{
		if (sdl_surfaceTexture->format->Rmask == 0x000000ff) glenumTextureFormat = GL_RGBA;
		else glenumTextureFormat = GL_BGRA;
	}
	else if (gliNumOfColors == 3) //if 3 no alpha channel
	{
		if (sdl_surfaceTexture->format->Rmask == 0x000000ff) glenumTextureFormat = GL_RGB;
		else glenumTextureFormat = GL_BGR;
	}
	else
	{
		cout << "warning: the image is not truecolor.. this will probably break";
		// this error should not go unhandled
	}
	#pragma endregion


	#pragma region Bind Texture
	/* Note OpenGL is a state machine any operations done
	   after this bind will be done on the texture with
	   the ID passed in as the 2nd parameter. */
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	#pragma endregion

	#pragma region Copy into texture pixel data
	/* https://www.opengl.org/sdk/docs/man3/xhtml/glTexImage2D.xml */
	glTexImage2D(GL_TEXTURE_2D,			// Texture type (Target)
		0,								// Mip-map (0 = first level)
		glenumTextureFormat,			// THIS textures format
		sdl_surfaceTexture->w,			// Texture width
		sdl_surfaceTexture->h,			// Texture height
		0,								// Border width
		glenumTextureFormat,			// Pixel format (SDL Surface pixel)
		GL_UNSIGNED_BYTE,				// Pixel data type (unsigned byte 0-255 for each pixel component)
		sdl_surfaceTexture->pixels);	// THIS pixel data (from SDL Surface)  
	#pragma endregion

	#pragma region Initial Texture states
	//Linear filtering on the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Set texturing addressing mode to CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Generates Mipmap for the texture
	glGenerateMipmap(GL_TEXTURE_2D);
	#pragma endregion


	return textureID;
}

GLuint loadTextureFromFont(const string& strFontFilename, int iPointSize, const string& strText)
{
	GLuint textureID = 0;

	#pragma region Load font using TTF_OpenFont
	TTF_Font *TTF_font = TTF_OpenFont(strFontFilename.c_str(), iPointSize);
	if (!TTF_font)
	{
		cout << "Unable to load font " << strFontFilename << " " << TTF_GetError();
		return textureID;
	}
	#pragma endregion

	/* create surface for function to render to
	http://sdl.beuc.net/sdl.wiki/SDL_ttf_Functions_Render
	   • Takes in a pointer to a loaded TTF_Font.
	   • string to be rendered.
	   • */
	SDL_Surface	*sdl_surfaceText = TTF_RenderText_Blended(TTF_font,
		strText.c_str(),
		{ 255, 255, 255 });

	textureID = convertSDLSurfaceToGLTexture(sdl_surfaceText);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Release memory allocated
	TTF_CloseFont(TTF_font);

	return textureID;
}

GLuint convertSDLSurfaceToGLTexture(SDL_Surface *sdl_surface)
{
	GLuint textureID = 0;
	GLint nOfColors = sdl_surface->format->BytesPerPixel;
	GLenum textureFormat = GL_RGB;
	GLenum internalFormat = GL_RGB8;
	if (nOfColors == 4) // contains an alpha channel
	{
		if (sdl_surface->format->Rmask == 0x000000ff)
		{
			textureFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
		}
		else
		{
			textureFormat = GL_BGRA;
			internalFormat = GL_RGBA8;
		}
	}
	else if (nOfColors == 3) // no alpha channel
	{
		if (sdl_surface->format->Rmask == 0x000000ff)
		{
			textureFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else
		{
			textureFormat = GL_BGR;
			internalFormat = GL_RGB8;
		}
	}
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, sdl_surface->w, sdl_surface->h, 0,
	textureFormat, GL_UNSIGNED_BYTE, sdl_surface->pixels);
	SDL_FreeSurface(sdl_surface);

	return textureID;
}