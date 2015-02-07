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
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
float fps;


Camera camera;
const float cameraMovSpeed = 0.05f;
const float camerarotDelta = 0.05f;


const int MAX_KEYS = 256;
bool keyPressed[MAX_KEYS];
int currMouseX, currMouseY;
int lastMouseX, lastMouseY;

ShaderProgram *blinnPhongShader;
ShaderProgram *cookTorShader;
ShaderProgram *celShader;
ShaderProgram *celOutlineShader; // first pass - render the outline of the object
ShaderProgram *blinnPhongTextureShader;
const int NUM_SHADERS = 3;
int currShader;

Model *monkey, *cube, *torus, *barbarian;
SQTTransform monkeyTransform, torusTransform, barbarianTransform, barbarianTransform2;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

float materialRoughness = 0.2f;
float materialRefraction = 0.3f;
float lineTickness = 0.05f;
glm::vec4 lineColor(0,0,0,1);
glm::vec3 lightDirection(-4.0f);

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


//--------------------- tweak bar bars ------------------
	TwBar *myBar;
	myBar = TwNewBar("Parameters");
	TwAddVarRW(myBar, "YRotation", TW_TYPE_FLOAT, &rotDelta, "");
	//TwAddButton(myBar, "Change Shader Type", changeShaderTypeCallback, NULL, NULL);

	// ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
	TwEnumVal shaderEV[NUM_SHADERS] = { {ShaderType::BLINNPHONG, "BlinnPhong"}, {ShaderType::COOKTOR, "CookTor"}, {ShaderType::CEL, "Cel"} };

	// Create a type for the enum shapeEV
	TwType shaderType = TwDefineEnum("ShapeType", shaderEV, NUM_SHADERS);
	// add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
	TwAddVarRW(myBar, "Shader", shaderType, &currShader, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
	TwAddVarRO(myBar, "FPS", TW_TYPE_FLOAT, &fps, "");
	   // Add 'g_LightDirection' to 'bar': this is a variable of type TW_TYPE_DIR3F which defines the light direction
    TwAddVarRW(myBar, "LightDir", TW_TYPE_DIR3F, &lightDirection, 
               " label='Light direction' opened=true help='Change the light direction.' ");

	TwAddVarRW(myBar, "Rroughness", TW_TYPE_FLOAT, &materialRoughness, "");
	TwAddVarRW(myBar, "Refraction", TW_TYPE_FLOAT, &materialRefraction, "");
	TwAddVarRW(myBar, "Line color", TW_TYPE_COLOR4F, &lineColor, "");
	TwAddVarRW(myBar, "Line tickness", TW_TYPE_FLOAT, &lineTickness, "");
//--------------------- end tweak bar bars ------------------


	//string blinnPhongVariables1[10] = {"position","normal","m_pvm","m_viewModel","m_normal","l_dir","diffuse","ambient","specular","shininess"};
	//vector<string> blinnPhongVariables2(blinnPhongVariables1,blinnPhongVariables1 + sizeof(blinnPhongVariables1)/sizeof(string));

	blinnPhongShader = ShaderManager::get().getShaderProgram("shaders/blinnPhongVS.glsl","shaders/blinnPhongFS.glsl",ShaderType::BLINNPHONG);
	//flatShader = ShaderManager::get().getShaderProgram("shaders/flatShaderVS.glsl","shaders/flatShaderFS.glsl",ShaderType::GOURAUD);
	cookTorShader = ShaderManager::get().getShaderProgram("shaders/cookTorVS.glsl","shaders/cookTorFS.glsl",ShaderType::COOKTOR);
	celShader = ShaderManager::get().getShaderProgram("shaders/celShaderVS.glsl","shaders/celShaderFS.glsl",ShaderType::CEL);
	celOutlineShader = ShaderManager::get().getShaderProgram("shaders/celOutlineShaderVS.glsl","shaders/celOutlineShaderFS.glsl",ShaderType::CEL_OUTLINE);
	blinnPhongTextureShader = ShaderManager::get().getShaderProgram("shaders/blinnPhongTextureVS.glsl","shaders/blinnPhongTextureFS.glsl",ShaderType::BLINNPHONG_TEXTURE);

	currShader = ShaderType::BLINNPHONG;

	monkey = new Model("monkey", "models/monkey/monkey.obj", blinnPhongShader);
	//cube = new Model("cube", "models/cube/cube.obj", blinnPhongShader);
	torus = new Model("torus", "models/torus/torus.obj", blinnPhongShader);
	barbarian = new Model("barbarian", "models/barbarian2/barbarian.obj", blinnPhongTextureShader);

	monkeyTransform = SQTTransform(glm::vec3(-2.0f,0,-6.0f),glm::vec3(1),glm::vec3(0));
	//cubeTransform = SQTTransform(glm::vec3(2,0,-6.0f),glm::vec3(1),glm::vec3(0,45.0f,60.0f));
	torusTransform = SQTTransform(glm::vec3(2,0,-6.0f),glm::vec3(1),glm::vec3(0,45.0f,60.0f));
	barbarianTransform = SQTTransform(glm::vec3(2,0,-6.0f),glm::vec3(1),glm::vec3(180.0f,45.0f,60.0f));
	barbarianTransform2 = SQTTransform(glm::vec3(0,-2,-6.0f),glm::vec3(1),glm::vec3(180.0f,45.0f,60.0f));
	projectionMatrix = glm::perspective(45.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
	//viewMatrix = glm::lookAt(glm::vec3(0,0,3.0f),glm::vec3(0),glm::vec3(0,1.0f,0));
	viewMatrix = camera.GetViewMatrix();
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);



}

void updateModels()
{
	lightDirection = glm::normalize(lightDirection);
	monkey->setMaterialRoughness(materialRoughness);
	torus->setMaterialRoughness(materialRoughness);
	barbarian->setMaterialRoughness(materialRoughness);

	monkey->setMaterialRefraction(materialRefraction);
	torus->setMaterialRefraction(materialRefraction);
	barbarian->setMaterialRefraction(materialRefraction);


	double delta = double(Timer::get().getLastInterval()) / 1000.0;
	torusTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	monkeyTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	barbarianTransform.pivotOnGlobalAxis(0,rotDelta*delta, 0);
	barbarianTransform2.pivotOnGlobalAxis(0,rotDelta*delta, 0);
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
	
	if(currShader == ShaderType::BLINNPHONG)
	{
		//blinn-phong has the same specs as gouroud. so does regular phong
		monkey->renderGouraud(blinnPhongShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
		torus->renderGouraud(blinnPhongShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
		barbarian->renderGouraud(blinnPhongShader, barbarianTransform2.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
	}
	else if(currShader == ShaderType::COOKTOR)
	{
		monkey->renderCookTor(cookTorShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
		torus->renderCookTor(cookTorShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
		barbarian->renderCookTor(cookTorShader, barbarianTransform2.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
	}
	else if(currShader == ShaderType::CEL)
	{
		monkey->renderCel(celShader, celOutlineShader, monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lineColor, lineTickness);
		torus->renderCel(celShader, celOutlineShader, torusTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lineColor, lineTickness);
		barbarian->renderCel(celShader, celOutlineShader, barbarianTransform2.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lineColor, lineTickness);
	}
	//monkey->render(monkeyTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
	//torus->render(torusTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);
	barbarian->renderBlinnPhongTexture(blinnPhongTextureShader, barbarianTransform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection);

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