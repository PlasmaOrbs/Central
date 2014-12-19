#include <iostream>
#include <GL/glew.h>
//maths headers
#include <glm/glm.hpp>
using glm::mat4;
using glm::vec4;
using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>
#include <CoreFoundation/CoreFoundation.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#elif WIN32
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <gl/GLU.h>
#endif

#include <vector>

#ifdef _DEBUG && WIN32
const std::string ASSET_PATH = "../assets/";
const std::string SHADER_PATH = "shaders/";
const std::string TEXTURE_PATH = "textures/";
const std::string FONT_PATH = "fonts/";
const std::string MODEL_PATH = "models/";
#elif __APPLE__
const std::string ASSET_PATH;
const std::string SHADER_PATH;
const std::string TEXTURE_PATH;
const std::string FONT_PATH;
const std::string MODEL_PATH;
#else
const std::string ASSET_PATH="/assets/";
const std::string SHADER_PATH = "shaders/";
const std::string TEXTURE_PATH = "textures/";
const std::string FONT_PATH = "fonts/";
const std::string MODEL_PATH = "models/";
#endif

//Our headers
#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "FBXLoader.h"
#include "FPSCameraController.h"
#include "BasicCameraController.h"

#include "PostProcessing.h"
#include "ColourFilters.h"

#include "SkyboxMaterial.h"
#include "BoxCollider.h"

#include "Input.h"
#include "Timer.h"

//SDL Window
SDL_Window * window = NULL;
//SDL GL Context
SDL_GLContext glcontext = NULL;

//Window Width
const int WINDOW_WIDTH = 640;
//Window Height
const int WINDOW_HEIGHT = 480;

bool running = true;
bool bDebug = false;

vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

BasicCameraController *basicCameraController;

std::vector<BoxCollider*> colliders;
std::vector<GameObject*> displayList;
GameObject * mainCamera;
GameObject * mainLight;
GameObject * skyBox=NULL;

PostProcessing postProcessor;


void CheckForErrors()
{
    GLenum error;
    do{
        error=glGetError();
    }while(error!=GL_NO_ERROR);
}

//Global functions
void InitWindow(int width, int height, bool fullscreen)
{
	//Create a window
	window = SDL_CreateWindow(
		"Game Engine",             // window title
		SDL_WINDOWPOS_CENTERED,     // x position, centered
		SDL_WINDOWPOS_CENTERED,     // y position, centered
		width,                        // width, in pixels
		height,                        // height, in pixels
		SDL_WINDOW_OPENGL           // flags
		);
}



void CleanUp()
{
	if (skyBox)
	{
		skyBox->destroy();
		delete skyBox;
		skyBox = NULL;
	}
    auto iter=displayList.begin();
	while(iter!=displayList.end())
    {
        (*iter)->destroy();
        if ((*iter))
        {
            delete (*iter);
            (*iter)=NULL;
            iter=displayList.erase(iter);
        }
        else
        {
            iter++;
        }
    }
    displayList.clear();
    
	postProcessor.destroy();
	// clean up, reverse order!!!
    Input::getInput().destroy();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void initInput()
{
    const std::string inputDBFilename=ASSET_PATH+"gamecontrollerdb.txt";
    Input::getInput().init(inputDBFilename);
}


//Function to initialise OpenGL
void initOpenGL()
{
    //Ask for version 3.2 of OpenGL
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
	//Create OpenGL Context
	glcontext = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}
    
    //Smooth shading
    glShadeModel( GL_SMOOTH );
    
    //clear the background to black
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    
    //Clear the depth buffer
    glClearDepth( 1.0f );
    
    //Enable depth testing
    glEnable( GL_DEPTH_TEST );
    
    //The depth test to go
    glDepthFunc( GL_LEQUAL );
    
    //Turn on best perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

//Function to set/reset viewport
void setViewport( int width, int height )
{
    
    //make sure height is always above 1
    if ( height == 0 ) {
        height = 1;
    }

    
    //Setup viewport
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );
}

void createSkyBox()
{
	Vertex triangleData[] = {
			{ vec3(-10.0f, 10.0f, 10.0f) },// Top Left
			{ vec3(-10.0f, -10.0f, 10.0f) },// Bottom Left
			{ vec3(10.0f, -10.0f, 10.0f) }, //Bottom Right
			{ vec3(10.0f, 10.0f, 10.0f) },// Top Right


			//back
			{ vec3(-10.0f, 10.0f, -10.0f) },// Top Left
			{ vec3(-10.0f, -10.0f, -10.0f) },// Bottom Left
			{ vec3(10.0, -10.0f, -10.0f) }, //Bottom Right
			{ vec3(10.0f, 10.0f, -10.0f) }// Top Right
	};


	GLuint indices[] = {
		//front
		0, 1, 2,
		0, 3, 2,

		//left
		4, 5, 1,
		4, 1, 0,

		//right
		3, 7, 2,
		7, 6, 2,

		//bottom
		1, 5, 2,
		6, 2, 1,

		//top
		5, 0, 7,
		5, 7, 3,

		//back
		4, 5, 6,
		4, 7, 6
	};

	//creat mesh and copy in

	Mesh * pMesh = new Mesh();
	pMesh->init();

	pMesh->copyVertexData(8, sizeof(Vertex), (void**)triangleData);
	pMesh->copyIndexData(36, sizeof(int), (void**)indices);

	Transform *t = new Transform();
	t->setPosition(0.0f, 0.0f, 0.0f);
	//load textures and skybox material + Shaders
	SkyBoxMaterial *material = new SkyBoxMaterial();
	material->init();
	
	std::string vsPath = ASSET_PATH + SHADER_PATH + "/skyVS.glsl";
	std::string fsPath = ASSET_PATH + SHADER_PATH + "/skyFS.glsl";
	material->loadShader(vsPath, fsPath);

	std::string posZTexturename = ASSET_PATH + TEXTURE_PATH+ "CloudyLightRaysFront2048.png";
	std::string negZTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysBack2048.png";
	std::string posXTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysLeft2048.png";
	std::string negXTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysRight2048.png";
	std::string posYTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysUp2048.png";
	std::string negYTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysDown2048.png";

	material->loadCubeTexture(posZTexturename, negZTexturename, posXTexturename, negXTexturename, posYTexturename, negYTexturename);
	//create gameobject but don't add to queue!
	skyBox = new GameObject();
	skyBox->setMaterial(material);
	skyBox->setTransform(t);
	skyBox->setMesh(pMesh);

	CheckForErrors();
}

void Initialise()
{
	createSkyBox();
	std::string vsPath = ASSET_PATH + SHADER_PATH + "/passThroughVS.glsl";
	std::string fsPath = ASSET_PATH + SHADER_PATH + "/colourFilterPostFS.glsl";

	//postProcessor.init(WINDOW_WIDTH, WINDOW_HEIGHT, vsPath, fsPath);

	mainCamera = new GameObject();
	mainCamera->setName("MainCamera");

	Transform *t = new Transform();
	t->setPosition(0.0f, 0.0f, 10.0f);

	mainCamera->setTransform(t);

	basicCameraController = new BasicCameraController();
	basicCameraController->setAspectRatio((float)(WINDOW_WIDTH / WINDOW_HEIGHT));
	basicCameraController->setFOV(45.0f);
	basicCameraController->setNearClip(0.1f);
	basicCameraController->setFarClip(1000.0f);

	mainCamera->setCamera(basicCameraController);
	
	displayList.push_back(mainCamera);

	mainLight = new GameObject();
	mainLight->setName("MainLight");

	t = new Transform();
	t->setPosition(0.0f, 0.0f, 0.0f);
	mainLight->setTransform(t);

	Light * light = new Light();
	mainLight->setLight(light);
	displayList.push_back(mainLight);

	//add light as a target for camera controller to look at
	basicCameraController->addGameObjectToTargets(mainLight);

	////////////////////////////////////////////////////////////////////////////////////////////
	// "armoredrecon" model : by Graham Alexander MacDonald
	// DIRECTIONAL LIGHT TEXTURE SHADER and HEIGHT MAPPING and SPECULAR HIGHLIGHT
	////////////////////////////////////////////////////////////////////////////////////////////

	std::string modelPath = ASSET_PATH + MODEL_PATH + "armoredrecon.fbx";

	//string holding address for texture
	std::string texturePath = ASSET_PATH + TEXTURE_PATH + "armoredrecon_diff.png";

	//string holding address for height map texture & specular map texture
	std::string heightMap = ASSET_PATH + TEXTURE_PATH + "armoredrecon_Height.png";
	std::string specularMap = ASSET_PATH + TEXTURE_PATH + "armoredrecon_N.png";

	GameObject * armordrecon = loadFBXFromFile(modelPath);
	armordrecon->setName("armordrecon");
	for (int i = 0; i < armordrecon->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();

		material->loadDiffuseMap(texturePath);

		//load the heightmap texture
		material->loadHeightMap(heightMap);

		//load the specular texture
		material->loadSpecularMap(specularMap);

		std::string vsPath = ASSET_PATH + SHADER_PATH + "/DirectionalLightTextureVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "/DirectionalLightTextureFS.glsl";
		material->loadShader(vsPath, fsPath);

		armordrecon->getChild(i)->setMaterial(material);
	}

	//add GameObject to vector of targets to look at in FPSController/BasicCameraController
	basicCameraController->addGameObjectToTargets(armordrecon);

	//Create BoxColliders
	BoxCollider *boxCollider = new BoxCollider();
	boxCollider->setLength(2.3f);
	boxCollider->setWidth(5.2f);
	boxCollider->setHeight(3.0f);

	//Set BoxCollider
	armordrecon->setBoxCollider(boxCollider);

	//BUG ERROR ON SETTING CENTRE BASED ON TRANSFORM
	armordrecon->getBoxCollider()->setCentre(vec3(-0.0f, -1.0f, 0.0f));

	armordrecon->getTransform()->setPosition(-0.0f, -1.0f, 0.0f);
	armordrecon->setLight(light);
	displayList.push_back(armordrecon);

	//Add BoxCollider to collider to detect for camera collision
	colliders.push_back(boxCollider);

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

	////////////////////////////////////////////////////////////////////////////////////////////
	
	//Start Game Clock
	Timer::getTimer().start();
}

//Function to update the game state
void update()
{
    Timer::getTimer().update();
	skyBox->update();
    //alternative sytanx
    for(auto iter=displayList.begin();iter!=displayList.end();iter++)
    {
        (*iter)->update();
    }

	//by Graham Alexander MacDonald
	for (auto iter = colliders.begin(); iter != colliders.end(); iter++)
	{
		vec3 v3PlayerPos = mainCamera->getTransform()->getPosition();
		bool bResult = (*iter)->checkForCollision(v3PlayerPos);

		if (bResult && !bDebug)
		{
			std::cout << "Player inside object at position ("
				<< v3PlayerPos.x << ","
				<< v3PlayerPos.y << ","
				<< v3PlayerPos.z << ")"
				<< std::endl;

			int iDir = basicCameraController->getDir();
			if (iDir > 0) basicCameraController->moveCamera("backward");
			else basicCameraController->moveCamera("forward");
		}
	}

	Input::getInput().update();
}

void renderGameObject(GameObject * pObject)
{
	if (!pObject)
		return;

	pObject->render();

	Mesh * currentMesh = pObject->getMesh();
	Transform * currentTransform = pObject->getTransform();
	//we know is going to be a standard material
	Material * currentMaterial = (Material*)pObject->getMaterial();

	if (currentMesh && currentMaterial && currentTransform)
	{
		currentMaterial->bind();
		currentMesh->bind();

		GLint MVPLocation = currentMaterial->getUniformLocation("MVP");
		GLint ModelLocation = currentMaterial->getUniformLocation("Model");
		GLint ambientMatLocation = currentMaterial->getUniformLocation("ambientMaterialColour");
		GLint ambientLightLocation = currentMaterial->getUniformLocation("ambientLightColour");
		GLint diffuseMatLocation = currentMaterial->getUniformLocation("diffuseMaterialColour");
		GLint diffuseLightLocation = currentMaterial->getUniformLocation("diffuseLightColour");
		GLint lightDirectionLocation = currentMaterial->getUniformLocation("lightDirection");
		GLint specularMatLocation = currentMaterial->getUniformLocation("specularMaterialColour");
		GLint specularLightLocation = currentMaterial->getUniformLocation("specularLightColour");
		GLint specularpowerLocation = currentMaterial->getUniformLocation("specularPower");
		GLint cameraPositionLocation = currentMaterial->getUniformLocation("cameraPosition");
		GLint diffuseTextureLocation = currentMaterial->getUniformLocation("diffuseMap");
		GLint specTextureLocation = currentMaterial->getUniformLocation("specMap");
		GLint bumpTextureLocation = currentMaterial->getUniformLocation("bumpMap");
		GLint heightTextureLocation = currentMaterial->getUniformLocation("heightMap");
		Camera * cam = mainCamera->getCamera();
		Light* light = mainLight->getLight();


		mat4 MVP = cam->getProjection()*cam->getView()*currentTransform->getModel();
		mat4 Model = currentTransform->getModel();

		vec4 ambientMaterialColour = currentMaterial->getAmbientColour();
		vec4 diffuseMaterialColour = currentMaterial->getDiffuseColour();
		vec4 specularMaterialColour = currentMaterial->getSpecularColour();
		float specularPower = currentMaterial->getSpecularPower();

		vec4 diffuseLightColour = light->getDiffuseColour();
		vec4 specularLightColour = light->getSpecularColour();
		vec3 lightDirection = light->getDirection();

		vec3 cameraPosition = mainCamera->getTransform()->getPosition();

		glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform4fv(ambientMatLocation, 1, glm::value_ptr(ambientMaterialColour));
		glUniform4fv(ambientLightLocation, 1, glm::value_ptr(ambientLightColour));

		glUniform4fv(diffuseMatLocation, 1, glm::value_ptr(diffuseMaterialColour));
		glUniform4fv(diffuseLightLocation, 1, glm::value_ptr(diffuseLightColour));
		glUniform3fv(lightDirectionLocation, 1, glm::value_ptr(lightDirection));

		glUniform4fv(specularMatLocation, 1, glm::value_ptr(specularMaterialColour));
		glUniform4fv(specularLightLocation, 1, glm::value_ptr(specularLightColour));

		glUniform3fv(cameraPositionLocation, 1, glm::value_ptr(cameraPosition));
		glUniform1f(specularpowerLocation, specularPower);

		glUniform1i(diffuseTextureLocation, 0);
		glUniform1i(specTextureLocation, 1);
		glUniform1i(bumpTextureLocation, 2);
		glUniform1i(heightTextureLocation, 3);

		glDrawElements(GL_TRIANGLES, currentMesh->getIndexCount(), GL_UNSIGNED_INT, 0);

		currentMaterial->unbind();
	}

	for (int i = 0; i < pObject->getChildCount(); i++)
	{
		renderGameObject(pObject->getChild(i));
	}
}

void renderSkyBox()
{
	skyBox->render();

	Mesh * currentMesh = skyBox->getMesh();
	SkyBoxMaterial * currentMaterial = (SkyBoxMaterial*)skyBox->getMaterial();
	if (currentMesh && currentMaterial)
	{
		Camera * cam = mainCamera->getCamera();

		currentMaterial->bind();
		currentMesh->bind();

		GLint cameraLocation = currentMaterial->getUniformLocation("cameraPos");
		GLint viewLocation = currentMaterial->getUniformLocation("view");
		GLint projectionLocation = currentMaterial->getUniformLocation("projection");
		GLint cubeTextureLocation = currentMaterial->getUniformLocation("cubeTexture");

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(cam->getProjection()));
        mat4 rotationY = glm::rotate(mat4(1.0f), mainCamera->getTransform()->getRotation().y, vec3(0.0f, 1.0f, 0.0f));
        mat4 rotationX = glm::rotate(mat4(1.0f), mainCamera->getTransform()->getRotation().x, vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(rotationY*rotationX));
		glUniform4fv(cameraLocation, 1, glm::value_ptr(mainCamera->getTransform()->getPosition()));
		glUniform1i(cubeTextureLocation, 0);

		glDrawElements(GL_TRIANGLES, currentMesh->getIndexCount(), GL_UNSIGNED_INT, 0);

		currentMaterial->unbind();
	}
	CheckForErrors();
}

//Function to render(aka draw)
void render()
{
	//Bind Framebuffer
	//postProcessor.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	glClearDepth(1.0f);
    //clear the colour and depth buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	renderSkyBox();

    //alternative sytanx
	for (auto iter = displayList.begin(); iter != displayList.end(); iter++)
	{
		renderGameObject((*iter));
	}
	
	//now switch to normal framebuffer
	//postProcessor.preDraw();
	//Grab stuff from shader
	GLint colourFilterLocation = postProcessor.getUniformVariableLocation("colourFilter"); 
	glUniformMatrix3fv(colourFilterLocation, 1, GL_FALSE, glm::value_ptr(SEPIA_FILTER));

	//draw
	//postProcessor.draw();

	//post draw
	//postProcessor.postDraw();
    
	SDL_GL_SwapWindow(window);
}



//Main Method
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
    
    // init everyting - SDL, if it is nonzero we have a problem
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "ERROR SDL_Init " <<SDL_GetError()<< std::endl;
        
        return -1;
    }
    
	int imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags) & (imageInitFlags)) != imageInitFlags) {
		std::cout << "ERROR SDL_Image Init " << IMG_GetError() << std::endl;
		// handle error
	}

	if (TTF_Init() == -1) {
		std::cout << "TTF_Init: " << TTF_GetError();
	}
    
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false);
    //Call our InitOpenGL Function
    initOpenGL();
    CheckForErrors();
    //Set our viewport
	setViewport(WINDOW_WIDTH, WINDOW_HEIGHT);

    initInput();
    
    Initialise();
    
    //Value to hold the event generated by SDL
    SDL_Event event;
    //Game Loop
	while (running)
    {
        //While we still have events in the queue
        while (SDL_PollEvent(&event)) {
            //Get event type
            //if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                //set our boolean which controls the loop to false
            //    running = false;
            //}
            switch (event.type)
            {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                {
                    running=false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    Input::getInput().getKeyboard()->setKeyDown(event.key.keysym.sym);
                    break;
                }
                case SDL_KEYUP:
                {
                    Input::getInput().getKeyboard()->setKeyUp(event.key.keysym.sym);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    Input::getInput().getMouse()->setMousePosition(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    Input::getInput().getMouse()->setMouseButtonDown(event.button.button);
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    Input::getInput().getMouse()->setMouseButtonUp(event.button.button);
                    break;
                }
                case SDL_CONTROLLERAXISMOTION:
                {
                    int controllerID=event.caxis.which;
                    short axisID = event.caxis.axis;
                    int axisValue = event.caxis.value;
                    //filter results between -3200 and 3200(are in the ‘dead zone’)
                    if (axisValue > Joypad::DeadzoneNeg && axisValue < Joypad::DeadzonePos)
                    {
                        axisValue = 0;
                    }
                    
                    Input::getInput().getJoypad(controllerID)->setAxisValue(axisID, axisValue);
                    
                    break;
                }
                case SDL_CONTROLLERBUTTONDOWN:
                {
                    int controllerID = event.cbutton.which;
                    short buttonID = event.cbutton.button;
                    
                    Input::getInput().getJoypad(controllerID)->setButtonDown(buttonID);
                    break;
                }
                case SDL_CONTROLLERBUTTONUP:
                {
                    int controllerID = event.cbutton.which;
                    short buttonID = event.cbutton.button;
                    
                    Input::getInput().getJoypad(controllerID)->setButtonUp(buttonID);
                    break;
                }
            }
        }
		update();
		render();       
    }
    

	CleanUp();
    
    return 0;
}