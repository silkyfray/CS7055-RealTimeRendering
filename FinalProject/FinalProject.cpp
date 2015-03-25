// Lab1.cpp : Defines the entry point for the console application.
//

#include "precompiled.h"
#include "common.h"

#include "Camera.h"
#include "Model.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"
#include "Timer.h"
#include "CubeMap.h"
#include "Utilities.h"
#include "Watercolor.h"

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AntTweakBar.h>


using namespace std;

const std::string WINDOW_NAME = "Watercolor Shading";
int windowId;
const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 800;
float fps;

Camera camera;
const float cameraMovSpeed = 0.05f;
const float camerarotDelta = 0.05f;
bool freeCamera;


const int MAX_KEYS = 256;
bool keyPressed[MAX_KEYS];
int currMouseX, currMouseY;
int lastMouseX, lastMouseY;

ShaderProgram *toon1Shader, *toon2Shader, *toon3Shader, *watercolorShader;
WatercolorInfo watercolorInfo;
int kernelSizeX, kernelSizeY;

Model *model1;
SQTTransform model1Transform;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);
glm::vec3 lightIntensity(0.5f,0.5f,0.5f);

float lineTickness = 0.05f;
glm::vec4 lineColor(0, 0, 0, 1);

float rotDelta = 0.0f;

GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil);
void init();
void updateModels();
void mouseCallback(int button, int state, int x, int y);
void keyboardCallback(unsigned char key, int x, int y);
void keyboardSpecialCallback(int key, int x, int y);
//The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed
void motionCallback(int x, int y);
//The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed
void passiveMotionCallback(int x, int y);
void displayCallback();
void idleCallback();
void handleKeys();

void TW_CALL changeShaderTypeCallback(void *clientData);


int main(int argc, char* argv[])
{
	//glut
	//setup context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
	windowId = glutCreateWindow(WINDOW_NAME.c_str());
	//register callbacks
	glutDisplayFunc(displayCallback);
	glutIdleFunc(idleCallback);
	
	//glutMouseFunc(mouseCallback);
	//glutKeyboardFunc(keyboardCallback);
	//glutSpecialFunc(keyboardSpecialCallback);
	//glutMotionFunc(motionCallback);
	//glutPassiveMotionFunc(passiveMotionCallback);

	// after GLUT initialization
	// directly redirect GLUT events to AntTweakBar
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT); 
	// send the ''glutGetModifers'' function pointer to AntTweakBar
	TwGLUTModifiersFunc(glutGetModifiers);

    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
       cout << "Error: " << glewGetErrorString(res) << "\n";
    }
	//glutInitContextVersion (4,4);
	//glutInitContextProfile ( GLUT_CORE_PROFILE );

	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	
	init();

	// Begin infinite event loop
	glutMainLoop();
	return 0;
}

void init()
{
	//tweakbar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

//--------------------- tweak bar bars ------------------
	TwBar *myBar;
	myBar = TwNewBar("Parameters");
	TwAddVarRW(myBar, "FPS", TW_TYPE_FLOAT, &fps, "");
	TwAddVarRO(myBar, "Pixel width", TW_TYPE_FLOAT, &watercolorInfo.pixelWidth, "");
	TwAddVarRO(myBar, "Pixel height", TW_TYPE_FLOAT, &watercolorInfo.pixelHeight, "");
	TwAddVarRW(myBar, "YRotation", TW_TYPE_FLOAT, &rotDelta, "");
	TwAddVarRW(myBar, "Light Direction", TW_TYPE_DIR3F, &lightDirection, " label='Light direction' opened=true help='Change the light direction.' ");
	TwAddSeparator(myBar, "Watercolor Params", NULL);
	TwAddVarRW(myBar, "Dry brush color", TW_TYPE_COLOR3F, &watercolorInfo.brushColor, "");
	TwAddVarRW(myBar, "Dry brush threshold", TW_TYPE_FLOAT, &watercolorInfo.brushThreshold, "step=0.01");
	TwAddVarRW(myBar, "kernel Size X", TW_TYPE_INT32, &kernelSizeX, "step=1");
	TwAddVarRW(myBar, "kernel Size Y", TW_TYPE_INT32, &kernelSizeY, "step=1");
	TwAddVarRW(myBar, "scaleFactorPigment", TW_TYPE_FLOAT, &watercolorInfo.scaleFactorPigment, "step=0.1");
	TwAddVarRW(myBar, "scaleFactorTurbulence", TW_TYPE_FLOAT, &watercolorInfo.scaleFactorTurbulence, "step=0.1");
	TwAddVarRW(myBar, "scaleFactorPaper", TW_TYPE_FLOAT, &watercolorInfo.scaleFactorPaper, "step=0.1");

	TwAddVarRW(myBar, "enableBrush", TW_TYPE_BOOL8, &watercolorInfo.enableBrush, "");
	TwAddVarRW(myBar, "enableWobble", TW_TYPE_BOOL8, &watercolorInfo.enableWobble, "");
	TwAddVarRW(myBar, "enableEdgeDarkening", TW_TYPE_BOOL8, &watercolorInfo.enableEdgeDarkening, "");
	TwAddVarRW(myBar, "enablePigment", TW_TYPE_BOOL8, &watercolorInfo.enablePigment, "");
	TwAddVarRW(myBar, "enableTurbulence", TW_TYPE_BOOL8, &watercolorInfo.enableTurbulence, "");
	TwAddVarRW(myBar, "enablePaper", TW_TYPE_BOOL8, &watercolorInfo.enablePaper, "");
	//uniform bool enableBrush, enableWobble, enableEdgeDarkening, enablePigment, enableTurbulence, enablePaper;

	//TwAddButton(myBar, "Change model", changeShaderTypeCallback, NULL, "");


//--------------------- end tweak bar bars ------------------


	//string blinnPhongVariables1[10] = {"position","normal","m_pvm","m_viewModel","m_normal","l_dir","diffuse","ambient","specular","shininess"};
	//vector<string> blinnPhongVariables2(blinnPhongVariables1,blinnPhongVariables1 + sizeof(blinnPhongVariables1)/sizeof(string));

	toon1Shader = ShaderManager::get().getShaderProgram("shaders/celShaderVS.glsl", "shaders/celShaderFS.glsl", ShaderType::CEL);
	toon2Shader = ShaderManager::get().getShaderProgram("shaders/celOutlineShaderVS.glsl", "shaders/celOutlineShaderFS.glsl", ShaderType::CEL_OUTLINE);
	//toon3Shader = ShaderManager::get().getShaderProgram("shaders/celShaderWVS.glsl", "shaders/celShaderWFS.glsl", ShaderType::CEL);
	watercolorShader = ShaderManager::get().getShaderProgram("shaders/watercolorVS.glsl", "shaders/watercolorFS.glsl", ShaderType::WATERCOLOR);

	watercolorInfo.abstractionShader = toon1Shader;
	watercolorInfo.watercolorShader = watercolorShader;

	//model1 = new Model("model1", "models/robot/exported/robot.obj", toon1Shader);
	model1 = new Model("model1", "models/monkey/monkey.obj", toon1Shader);
	model1Transform = SQTTransform(glm::vec3(0, 0, -2), glm::vec3(0.5), glm::vec3(0));
	//model1Transform = SQTTransform(glm::vec3(0, -1, -2), glm::vec3(.5), glm::vec3(0));
	//model1 = new Model("model1", "models/robot/exported/robot.dae", toon1Shader);
	//model1Transform = SQTTransform(glm::vec3(0, 0, -5), glm::vec3(0.03), glm::vec3(0));

	viewMatrix = camera.GetViewMatrix();
	projectionMatrix = glm::perspective(45.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);


	//WATERCOLOR STUFF
	GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// Positions   // TexCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	// Setup screen VAO
	GLuint quadVBO;
	glGenVertexArrays(1, &(watercolorInfo.quadVAO));
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(watercolorInfo.quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(watercolorShader->getVariableLocation("position"));
	glVertexAttribPointer(watercolorShader->getVariableLocation("position"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(watercolorShader->getVariableLocation("texCoord"));
	glVertexAttribPointer(watercolorShader->getVariableLocation("texCoord"), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);


	// Framebuffers
	glGenFramebuffers(1, &(watercolorInfo.frameBuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, watercolorInfo.frameBuffer);
	// Create a color attachment texture
	watercolorInfo.textureColorBuffer = generateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, watercolorInfo.textureColorBuffer, 0);
	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // Use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
	// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	watercolorInfo.pigmentTexture = Utilities::loadTextureFromFile("textures/grayscale/noise.jpg", false, false, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	watercolorInfo.turbulenceTexture = Utilities::loadTextureFromFile("textures/grayscale/clouds-grayscale.jpg", false, false, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	watercolorInfo.paperTexture = Utilities::loadTextureFromFile("textures/grayscale/rough-paper-grayscale2.jpg", false, false, GL_REPEAT, GL_LINEAR, GL_LINEAR);

	watercolorInfo.pixelWidth = 1.0f / (float)SCREEN_WIDTH;
	watercolorInfo.pixelHeight = 1.0f / (float)SCREEN_HEIGHT;

	watercolorInfo.brushColor = glm::vec3(1.0f);
	watercolorInfo.brushThreshold = 0.98f;
	kernelSizeX = 2;
	kernelSizeY = 2;
	watercolorInfo.kernelOffsetX = kernelSizeX * watercolorInfo.pixelWidth;
	watercolorInfo.kernelOffsetY = kernelSizeY * watercolorInfo.pixelHeight;

	watercolorInfo.scaleFactorPigment = 1.0;
	watercolorInfo.scaleFactorTurbulence = 3.0;
	watercolorInfo.scaleFactorPaper = 5.0;

	watercolorInfo.enableBrush = true;
	watercolorInfo.enableWobble = true;
	watercolorInfo.enableEdgeDarkening = true;
	watercolorInfo.enablePigment = true;
	watercolorInfo.enableTurbulence = true;
	watercolorInfo.enablePaper = true;
}

void updateModels()
{
	lightDirection = glm::normalize(lightDirection);
	double delta = double(Timer::get().getLastInterval()) / 1000.0;
	model1Transform.pivotOnLocalAxisDegrees(0,rotDelta*delta, 0);
	fps = Timer::get().getFrameRate();
	
	watercolorInfo.kernelOffsetX = kernelSizeX * watercolorInfo.pixelWidth;
	watercolorInfo.kernelOffsetY = kernelSizeY * watercolorInfo.pixelHeight;

}

void handleKeys()
{
	double delta = double(Timer::get().getLastInterval()) / 1000.0;

	//camera
	if(keyPressed['w'])
	{
		camera.ProcessKeyboard(Direction::FORWARD,delta);
	}
	if(keyPressed['s'])
	{
		camera.ProcessKeyboard(Direction::BACKWARD,delta);
	}
	if(keyPressed['a'])
	{
		camera.ProcessKeyboard(Direction::LEFT,delta);
	}
	if(keyPressed['d'])
	{
		camera.ProcessKeyboard(Direction::RIGHT,delta);
	}
	//shaders
	if(keyPressed[GLUT_KEY_RIGHT])
	{
	}

	for (int i = 0; i < MAX_KEYS; i++)
	{
		keyPressed[i] = false;
	}

}
void displayCallback()
{

	Timer::get().updateInterval();
	handleKeys();
	updateModels();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.8,0.8,0.8,1.0);
    glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	viewMatrix = camera.GetViewMatrix();
	//model1->renderCel(toon1Shader, toon2Shader, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lineColor, lineTickness);
	model1->renderWatercolor(watercolorInfo, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
	TwDraw();  // draw the tweak bar(s)
	glutSwapBuffers();
	//cout << "Frame Rate: " << Timer::get().getFrameRate() << endl;

}

void idleCallback()
{
	float delta = float(Timer::get().getLastInterval());
	float offsetX = float(currMouseX - lastMouseX) * delta;
	float offsetY = float(currMouseY - lastMouseY) * delta;
	lastMouseX = currMouseX;
	lastMouseY = currMouseY;
	camera.ProcessMouseMovement(offsetX, -offsetY);
	//cout << "camera x: " << camera.Front.x
	//glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	glutPostRedisplay();
}

void mouseCallback(int button, int state, int x, int y)
{
	//One time presses
	cout << "x: " << x << " y: " << y << endl;
	//Hold presses
	if(state == GLUT_DOWN) // remember that the glut mouse buttons overlap with the glut special buttons in terms of integer assignment
	{
		keyPressed[button] = true;
	}
	else //GLUT_UP
	{
		keyPressed[button] = false;
	}
}

void keyboardCallback(unsigned char key, int x, int y)
{
	//One time presses
	switch(key)
	{
	case 27: // escape key
		glutDestroyWindow(windowId);
		TwTerminate();
		exit(0);
		break;
	}

	//Hold presses
	keyPressed[key] = true;
}
void keyboardSpecialCallback(int key, int x, int y)
{
	//Hold presses
	keyPressed[key] = true;
}

void motionCallback(int x, int y)
{
	currMouseX = x;
	currMouseY = y;	

}

void passiveMotionCallback(int x, int y)
{
	motionCallback(x,y);
}

#pragma region AntTweakBarCallbacks
void TW_CALL changeShaderTypeCallback(void *clientData)
{
}

#pragma endregion

// Generates a texture that is suited for attachments to a framebuffer
GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil)
{
	// What enum to use?
	GLenum attachment_type;
	if (!depth && !stencil)
		attachment_type = GL_RGB;
	else if (depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if (!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;
	else
		attachment_type = GL_DEPTH_STENCIL;
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, SCREEN_WIDTH, SCREEN_HEIGHT, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}