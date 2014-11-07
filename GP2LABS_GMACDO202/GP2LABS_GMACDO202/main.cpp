#pragma region Includes & using
#include <iostream>
#include <GL/glew.h>
#include <SDL.h> 
#include <SDL_opengl.h>
#include <gl\GLU.h>
#include <SDL_Image.h> //for texturing and font
#include <SDL_ttf.h> // for font

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

using namespace std;

/* Used to build a directory we can load shaders dependant
   on what version of the application we are building*/
//#ifdef __DEBUG && WIN32 //if debug build up a level from debug directory
#ifdef WIN32
const string ASSET_PATH = "../assets";
const string SHADER_PATH = "/shaders";
const string TEXTURE_PATH = "/textures";
const string FONT_PATH = "/fonts";
#elif __APPLE__
const string ASSET_PATH;
const string SHADER_PATH;
#else //else we build relative to .exe
const string ASSET_PATH = "../assets";
const string SHADER_PATH = "/shaders";
const string TEXTURE_PATH = "/textures";
const string FONT_PATH = "/fonts";
#endif

//maths	headers
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma endregion

#pragma region Global Variables
//Pointer to our SDL Windows
SDL_Window * window;

/*
SDL GL Context while not used directly is needed in 
circumstances but is a prerequisite before we start interacting
with OpenGL
*/
SDL_GLContext sdl_glcontext = NULL;

/*
Passed into the functions which bind the buffer to the pipeline
and also when copying data into the buffer
- filled out when generating the buffer
*/
GLuint gluiVBO;

/* Integer Element buffer object*/
GLuint gluiEBO;

//holds together VBO, EBO and VertexAttribute pointers (OpenGL version 3.2+)
GLuint gluiVAO;

/* Shader program global value*/
GLuint gluiShaderProgram = 0;

/* Texture object */
GLuint gluiTexture = 0;

/* Font object*/
GLuint gluiFontTexture = 0;

//Constants to control window creation
const int iWINDOW_WIDTH = 640;
const int iWINDOW_HEIGHT = 480;

//Controls game loop
bool bRunning = true;

//matrices
mat4 m4ViewMatrix;
mat4 m4ProjMatrix;
mat4 m4WorldMatrix;

Vertex CubeData[] =
{
	//Front
	{ vec3(-0.5f, 0.5f, 0.5f), vec2(0.0f, 0.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top Left
	{ vec3(-0.5f, -0.5f, 0.5f), vec2(0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f) }, // Bottom Left
	{ vec3(0.5f, -0.5f, 0.5f), vec2(1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f) }, //Bottom Right
	{ vec3(0.5f, 0.5f, 0.5f), vec2(1.0f, 0.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f) } //Top Right
};

GLuint gluiArrayOfIndices[] =
{
	//front
	0, 1, 2,
	0, 3, 2,
};

float fPositionX = 0;
float fPositionY = 0;
float fAdjustPositionBy = 0.1f;

#pragma endregion

#pragma region Global Functions
///////////////////////////////////////////////////////////

/*
This function takes in two integers(window width & height)
and a boolean(determines if the window should be fullscreen
or windowed).

flags control how our window is created(fullscreen, support
for OpenGL etc). (SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL)
*/
void InitWindow(int width, int height, bool fullscreen)
{
	//Create a window
	window = SDL_CreateWindow(
		"Boom",						// window title
		SDL_WINDOWPOS_CENTERED,     // x position, centered
		SDL_WINDOWPOS_CENTERED,     // y position, centered
		width,                      // width, in pixels
		height,                     // height, in pixels
		SDL_WINDOW_OPENGL           // flags
		);

}

///////////////////////////////////////////////////////////

/*
This function will be called when we are exiting our Game
Loop and will be used to cleanup any memory allocated

Memory clean up is carried out in reverse order to memory
allocated
*/
void CleanUp()
{
	glDeleteTextures(1, &gluiFontTexture);
	glDeleteTextures(1, &gluiTexture);

	glDeleteProgram(gluiShaderProgram);

	//glDeleteBuffers(NUMBER_OF_BUFFERS, BUFFER_TO_CLEANED_UP)
	glDeleteBuffers(1, &gluiEBO);
	glDeleteBuffers(1, &gluiVBO); //Deletes number of buffers specified

	glDeleteVertexArrays(1, &gluiVAO);

	SDL_GL_DeleteContext(sdl_glcontext); //Clean up OpenGL functionality
	SDL_DestroyWindow(window); //clean up window memory
	SDL_Quit(); //clean up SDL libary memory
}

///////////////////////////////////////////////////////////

/*

*/
void InitOpenGL()
{
	/* Ask for version 3.2 of OpenGL
	   reference https://www.opengl.org/wiki/Core_And_Compatibility_in_Contexts */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	//Create OpenGL Context
	sdl_glcontext = SDL_GL_CreateContext(window);
	//Something went wrong in creating the context, if it is still NULL
	if (!sdl_glcontext)
	{
		cout << "Error Creating OpenGL Context " << SDL_GetError() << endl;
	}

	/* Queries all supported extensions by the graphics driver
	when glewInit is called */
	glewExperimental = GL_TRUE;
	//Register all extensions
	GLenum glenumError = glewInit();
	if (GLEW_OK != glenumError)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		cout << "Error: " << glewGetErrorString(glenumError) << endl;
	}

	//Smooth shading
	glShadeModel(GL_SMOOTH);

	//clear the background to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clear the depth buffer to 1.0
	glClearDepth(1.0f);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);

	//The depth test to use
	glDepthFunc(GL_LEQUAL);

	//Turn on best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

///////////////////////////////////////////////////////////

/*
This function creates and fills a Vertbex Buffer Object with
some Vertex Data
*/
void InitGeometry()
{
	// [1] CREATING VERTEX ARRAY OBJECT
	
	glGenVertexArrays(1, &gluiVAO);
	glBindVertexArray(gluiVAO);
	
	// [2] CREATING VERTEX BUFFER

	//glGlenBuffers(NUMBER_OF_BUFFERS, POINTER_ARRAY_OF_INTS)
	glGenBuffers(1, &gluiVBO); //Create Buffers

	//glBindBuffer(BUFFER_TYPE, PARAMETER_TO_BE_BOUND_TO_PIPELINE)
	glBindBuffer(GL_ARRAY_BUFFER, gluiVBO);// Make the new VBO active

	//Copy Vertex Data to VBO
	glBufferData(
		GL_ARRAY_BUFFER, //Type of buffer
		24 * sizeof(float), //Size of data being copied
		CubeData, //Data being copied
		GL_STATIC_DRAW); //Hint to openGL on what to do with the data

	// [3] CREATING ELEMENT BUFFER

	//create buffer
	glGenBuffers(1, &gluiEBO);
	//Make the EBO active
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gluiEBO);
	//Copy Index data to the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		36 * sizeof(int),
		gluiArrayOfIndices,
		GL_STATIC_DRAW);
}

///////////////////////////////////////////////////////////

/* for 2d sprite */
void InitGeometryFromTexture(GLuint textureID)
{
	int iWidth, iHeight;

	/* bind texture to pipeline */
	glBindTexture(GL_TEXTURE_2D, textureID);
	/* Retrieve height and width from texture */
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iHeight);

	Vertex spriteData[] =
	{
		//Front
		{ vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) }, // Top Left
		{ vec3(0.0f, iHeight, 0.0f), vec2(0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) }, // Bottom Left
		{ vec3(iWidth, iHeight, 0.0f), vec2(1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) }, //Bottom Right
		{ vec3(iWidth, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f) } //Top Right
	};

	GLuint gluiArrayForSpriteIndices[] =
	{
		//front
		0, 1, 2,
		0, 3, 2,
	};

	glGenVertexArrays(1, &gluiVAO);
	glBindVertexArray(gluiVAO);
	
	// create buffer
	glGenBuffers(1, &gluiVBO);
	// Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, gluiVBO);
	// Copy Vertex Data to VBO
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), spriteData, GL_STATIC_DRAW);

	// create buffer
	glGenBuffers(1, &gluiEBO);
	//make EBO active
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gluiEBO);
	//copy Vertex Data to the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), gluiArrayOfIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)sizeof(vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)sizeof(vec3) + sizeof(vec2));
}

///////////////////////////////////////////////////////////

void CreateShader()
{
	GLuint gluiVertexShaderProgram = 0;
	string vsPath = ASSET_PATH + SHADER_PATH + "/textureVS.glsl";
	gluiVertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);

	GLuint gluiFragmentShaderProgram = 0;
	string fsPath = ASSET_PATH + SHADER_PATH + "/textureFS.glsl";
	gluiFragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);

	
	gluiShaderProgram = glCreateProgram(); //Create shader program

	//Attach vertex shader program
	glAttachShader(gluiShaderProgram, gluiVertexShaderProgram);
	//Attach fragment shader program
	glAttachShader(gluiShaderProgram, gluiFragmentShaderProgram);

	glLinkProgram(gluiShaderProgram); //link shader
	checkForLinkErrors(gluiShaderProgram); //check for link errors

	//Binds the location 0 to vertexPosition(shader input variable)
	glBindAttribLocation(gluiShaderProgram, 0, "vertexPosition");
	glBindAttribLocation(gluiShaderProgram, 1, "vertexTexCoords");
	glBindAttribLocation(gluiShaderProgram, 2, "vertexColour");

	//Delete Vertex and fragment shader program
	glDeleteShader(gluiFragmentShaderProgram);
	glDeleteShader(gluiVertexShaderProgram);
}

///////////////////////////////////////////////////////////

void CreateTexture()
{
	string strTexturePath = ASSET_PATH + TEXTURE_PATH + "/texture.png";
	gluiTexture = loadTextureFromFile(strTexturePath);
}

///////////////////////////////////////////////////////////

void CreateFontTexture()
{
	string strTexturePath = ASSET_PATH + FONT_PATH + "OrastorStd.otf";
	gluiFontTexture = loadTextureFromFont(strTexturePath, 64, "Hello");
}

///////////////////////////////////////////////////////////

/*

*/
void SetViewport(int iWidth, int iHeight)
{
	//make sure height is always above 0
	if (iHeight == 0) {
		iHeight = 1;
	}

	//Setup viewport
	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);

    #pragma region OLD FUNCTIONALITY [1]
	/* Previous version of OpenGL

	these old functions all deal with transformations which has
	been taken over by the Vertex Shader

	//screen ration
	GLfloat glfRatio;

	//calculate screen ration
	glfRatio = (GLfloat)width / (GLfloat)height;

	//Change to projection matrix mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Calculate perspective matrix, using glu library functions
	gluPerspective(45.0f, glfRatio, 0.1f, 100.0f);

	//Swith to ModelView
	glMatrixMode(GL_MODELVIEW);

	//Reset using the Indentity Matrix
	glLoadIdentity();
	*/
	#pragma endregion

}

///////////////////////////////////////////////////////////

//Function to draw
void Render()
{
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clear the colour and depth buffer (actual clearing)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(gluiVAO);

	//Binds shader program to the pipeline
	glUseProgram(gluiShaderProgram);

	/* below 3 lines grabbing the shader program*/
	GLint gliMVPLocation = glGetUniformLocation(gluiShaderProgram, "MVP");
	mat4 MVP = m4ProjMatrix * m4ViewMatrix * m4WorldMatrix;
	glUniformMatrix4fv(gliMVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));

	GLint texture0Location = glGetUniformLocation(gluiShaderProgram, "texture0");
	glActiveTexture(GL_TEXTURE0); //activate texture unit to use texture
	glBindTexture(GL_TEXTURE_2D, gluiTexture); //bind texture as a GL_TEXTURE_2D
	glUniform1i(texture0Location, 0); //tell shader to access this texture
	
	#pragma region OLD FUNCTIONALITY [1]
	/*

	Vertex shader has taken over this functionality
	

	//the 3 parameter is now filled out, the pipeline needs to know the size of
	//each vertex
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
	
	//The last parameter basically says that the colours start 3 floats into
	// each element of the array
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), (void**)(3 * sizeof(float)));

	//Establish array contains vertices & colours
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);

	//Reset using the Indentity Matrix
	glLoadIdentity();

	// CAMERA

	Calculate view matrix and push it onto the top of the matrix stack
	For every object to be rendered in 3D

	• 1st three	parameters is the camera position in 3D space(x,y,z)
	• 2nd set of three parameters is the centre(look at point) in 3D space(x,y,z)
	• 3rd set of three parameter is the Up axis of the camera(x,y,z)

	gluLookAt(0.0, 0.0, 0.0, 0.0, 1.0, -1.0f, 0.0, 1.0, 0.0);

	//translate
	glTranslatef(-2.0f, 0, -6.0f);

	//Actually draw the triangle, giving the number of vertices provided
	/* glDrawArrays(GL_TRIANGLES,
	0,
	sizeof(CubeData) / sizeof(Vertex));*/
	#pragma endregion

	//Draw elements
	glDrawElements(GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		0);

	#pragma region OLD FUNCTIONALITY [1]
	//Actually draw the triangle, giving the number of vertices provided
	//glDrawArrays(GL_TRIANGLES, 0, sizeof(triangleData) / sizeof(Vertex));  
	#pragma endregion


	#pragma region Primative Functionality Triangle
	//Begin drawing triangles
	//glBegin(GL_TRIANGLES);
	/* This sets the colour of the vertices where each component is a floating point
	   number(0.0f – 1.0f) and represents red, green and blue. */
	//glColor3f(1.0f, 0.0f, 0.0f); //Colour of the vertices (RED)
	//This specifies a Vertex using an x.y and z positional values.
	//glVertex3f(0.0f, 1.0f, 0.0f); // Top
	//glColor3f(0.0f, 1.0f, 0.0f); //(BLUE)
	//glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
	//glColor3f(0.0f, 0.0f, 1.0f); //(GREEN)
	//glVertex3f(1.0f, -1.0f, 0.0f); // Bottom Right
	//This ends the drawing process
	//glEnd();
	#pragma endregion

	//require to swap the back and front buffer
	SDL_GL_SwapWindow(window);
}

///////////////////////////////////////////////////////////

//Function to update game state
void Update()
{
	/*m4ProjMatrix = glm::perspective(45.0f,
		(float)iWINDOW_WIDTH /(float)iWINDOW_HEIGHT, 0.1f, 100.0f); */

	/* Orthographic projection */
	m4ProjMatrix = glm::ortho(0.0f, // Left coordinate of our projection (0.0f = LHS of screen)
		(float)iWINDOW_WIDTH,		// Right coordinate of our projection (Window width = RHS of screen)
		(float)iWINDOW_HEIGHT,		// Top coordinate (Window height = bottom of screen)
		0.0f,						// Bottom coordinate (0.0f = top of screen)
		0.1f,						// Near clip (Vertexs less than = clipped)
		100.0f);					// Far clip (Vertexs more than = clipped)

	m4ViewMatrix = glm::lookAt(vec3(0.0f, 0.0f, 10.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	m4WorldMatrix = glm::translate(mat4(1.0f),
		vec3(0.0f, 0.0f, 0.0f));
}

///////////////////////////////////////////////////////////

/*
Main Method - Entry Point:
The entry point receives two variables; an interger which
tracks the number of command line arguments sent to the
application and a pointer to an array of characters(strings)
which contain the command line arguments. The function
returns an integer which usually determines if the
application has run successfully(zero) or
unsuccessfully(negative).

SDL_Init [1]
https://wiki.libsdl.org/SDL_Init reference
*/
int main(int argc, char * arg[])
{
	// Setup asset path, no real work required for Windows. Mac needs to load assets from a bundle
	// ----------------------------------------------------------------------------
	// http://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
	#ifdef __APPLE__
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		// error!
	}
	CFRelease(resourcesURL);

	chdir(path);
	std::cout << "Current Path: " << path << std::endl;
	#endif

	// [1] init everything - SDL, if it is nonzero we have a problem
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cout << "ERROR SDL_Init " << SDL_GetError() << endl;

		return -1;
	}
	/* Initilaises SDL image with support for JPG and PNG*/
	int	imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int	returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags)&	(imageInitFlags)) != imageInitFlags)
	{
		cout << "ERROR	SDL_Image	Init	" << IMG_GetError() << endl;
		// handle error
	}

	if (TTF_Init() == -1)	{
		std::cout << "ERROR	TTF_Init:	" << TTF_GetError();
	}

	//Initiate the window
	InitWindow(iWINDOW_WIDTH, iWINDOW_HEIGHT, false);

	//Call our InitOpenGL Function
	InitOpenGL();

	//Set our viewport
	SetViewport(iWINDOW_WIDTH, iWINDOW_HEIGHT);
	
	//Call our Geometry
	InitGeometry();

	CreateTexture();

	CreateFontTexture();

	InitGeometryFromTexture(gluiFontTexture);

	//Create and link shaders
	CreateShader();

	/*
	SDL_Event will be used to hold the event data generated by
	our Window. The user interacts with the window.
	*/
	SDL_Event sdl_event;

	#pragma region Game Loop
	while (bRunning)
	{
		
		#pragma region Event Queue
		/* Peek and Poll: Peek is used to check the event but will not
		   remove it; Poll will check the event but will also remove it
		   from the queue
		   
		   If there are events in the queue then the function will return
		   a positive value. The function will also fill out the SDL_Event
		   variable with the details of the event. */
		while (SDL_PollEvent(&sdl_event))
		{
			#pragma region Event Procedures
			if (sdl_event.type == SDL_QUIT || sdl_event.type == SDL_WINDOWEVENT_CLOSE) {
				//set our boolean which controls the game loop to false
				bRunning = false;
			}

			switch (sdl_event.type)
			{
				case SDL_KEYDOWN:

					//http://wiki.libsdl.org/SDL_Keycode
					switch (sdl_event.key.keysym.scancode)
					{
						#pragma region Number Keys 0 - 9
						case SDL_SCANCODE_0:
							
						case SDL_SCANCODE_1:
							fPositionX++;
						case SDL_SCANCODE_2:
							fPositionX--;
						case SDL_SCANCODE_3:
							fPositionY++;
						case SDL_SCANCODE_4:
							fPositionY--;
						case SDL_SCANCODE_5:
						case SDL_SCANCODE_6:
						case SDL_SCANCODE_7:
						case SDL_SCANCODE_8:
						case SDL_SCANCODE_9:
						#pragma endregion

						default:
							break;
					}

				case SDL_KEYUP:
				default:
					break;
			}
			#pragma endregion

		}
		#pragma endregion

		Update();
		Render();
	}
	#pragma endregion


	//Release memory allocated
	CleanUp();

	return 0;
}

///////////////////////////////////////////////////////////
#pragma endregion


