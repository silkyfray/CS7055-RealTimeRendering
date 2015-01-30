// Lab1.cpp : Defines the entry point for the console application.
//

#include "precompiled.h"
#include "common.h"

#include "Camera.h"
#include "Model.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"
#include "Timer.h"

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AntTweakBar.h>

using namespace std;

const std::string WINDOW_NAME = "GLSL Shading";
int windowId;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
float fps;


Camera camera;
const float cameraMovSpeed = 0.05f;
const float camerarotDelta = 0.05f;


const int MAX_KEYS = 256;
bool keyPressed[MAX_KEYS];
int currMouseX, currMouseY;
int lastMouseX, lastMouseY;

ShaderProgram *flatShader;
ShaderProgram *gouraudShader;
ShaderProgram *phongShader;
ShaderProgram *celShader;

const int NUM_SHADERS = 3;
int currShader;

Model *monkey, *cube, *torus;
SQTTransform monkeyTransform, cubeTransform, torusTransform;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::vec3 lightPositionWorld(4.0f, 4.0f, 4.0f);

float rotDelta = 30.0f;


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
	TwBar *myBar;
	myBar = TwNewBar("Parameters");
	TwAddVarRW(myBar, "YRotation", TW_TYPE_FLOAT, &rotDelta, "");
	//TwAddButton(myBar, "Change Shader Type", changeShaderTypeCallback, NULL, NULL);

	// ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
	TwEnumVal shaderEV[NUM_SHADERS] = { {ShaderType::GOURAUD, "Gouraud"}, {ShaderType::PHONG, "Phong"}, {ShaderType::CEL, "Cel"} };

	// Create a type for the enum shapeEV
	TwType shaderType = TwDefineEnum("ShapeType", shaderEV, NUM_SHADERS);
	// add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
	TwAddVarRW(myBar, "Shader", shaderType, &currShader, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
	TwAddVarRO(myBar, "FPS", TW_TYPE_FLOAT, &fps, "");
	gouraudShader = ShaderManager::get().getShaderProgram("shaders/gouraudShaderVS.glsl","shaders/gouraudShaderFS.glsl",ShaderType::GOURAUD);
	//flatShader = ShaderManager::get().getShaderProgram("shaders/flatShaderVS.glsl","shaders/flatShaderFS.glsl",ShaderType::GOURAUD);
	phongShader = ShaderManager::get().getShaderProgram("shaders/phongShaderVS.glsl","shaders/phongShaderFS.glsl",ShaderType::PHONG);
	celShader = ShaderManager::get().getShaderProgram("shaders/celShaderVS.glsl","shaders/celShaderFS.glsl",ShaderType::CEL);

	currShader = 0;

	monkey = new Model("monkey", "models/monkey/monkey.obj", phongShader);
	//cube = new Model("cube", "models/cube/cube.obj", gouraudShader);
	torus = new Model("cube", "models/torus/torus.obj", phongShader);

	monkeyTransform = SQTTransform(glm::vec3(-2.0f,0,-6.0f),glm::vec3(1),glm::vec3(0,45.0f,0.0f));
	//cubeTransform = SQTTransform(glm::vec3(2,0,-6.0f),glm::vec3(1),glm::vec3(0,45.0f,60.0f));
	torusTransform = SQTTransform(glm::vec3(2,0,-6.0f),glm::vec3(1),glm::vec3(0,45.0f,60.0f));

	projectionMatrix = glm::perspective(45.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
	//viewMatrix = glm::lookAt(glm::vec3(0,0,3.0f),glm::vec3(0),glm::vec3(0,1.0f,0));
	viewMatrix = camera.GetViewMatrix();
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void updateModels()
{
	double delta = double(Timer::get().getLastInterval()) / 1000.0;
	cubeTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	torusTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	monkeyTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	fps = Timer::get().getFrameRate();
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
	glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	viewMatrix = camera.GetViewMatrix();
	
	if(currShader == ShaderType::GOURAUD)
	{
		monkey->renderGouraud(gouraudShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
		torus->renderGouraud(gouraudShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
	}
	else if(currShader == ShaderType::PHONG)
	{
		monkey->renderPhong(phongShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
		torus->renderPhong(phongShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
	}
	else if(currShader == ShaderType::CEL)
	{
		monkey->renderCel(celShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
		torus->renderCel(celShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, lightPositionWorld);
	}

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
	currShader++;
	if(currShader > NUM_SHADERS)
		currShader = 0;
	cout << "Current shader type: " << currShader << endl;
}

#pragma endregion