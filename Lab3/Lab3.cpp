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

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AntTweakBar.h>

using namespace std;

const std::string WINDOW_NAME = "GLSL Shading";
int windowId;
const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 800;
float fps;

Camera camera;
const float cameraMovSpeed = 0.05f;
const float camerarotDelta = 0.05f;
bool freeCamera;

bool model1question;

const int MAX_KEYS = 256;
bool keyPressed[MAX_KEYS];
int currMouseX, currMouseY;
int lastMouseX, lastMouseY;

ShaderProgram *reflectRefractShader, *textureCubeShader, *normalMapShader, *reflectRefractNormalMapShader;
const string diffuseTexStr("diffuse texture"); 
const string normalTexStr("diffuse texture"); 
const string diffuseNormalTexStr("diffuse + normal texture"); 
const string FullStr("diffuse + normal texture + transmittance"); 

const int NUM_SHADERS = 1;
int currShader;

Model *model1;
Model *model2;
SQTTransform model1Transform;
SQTTransform model2Transform;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);
glm::vec3 lightIntensity(0.5f,0.5f,0.5f);

float rotDelta = 30.0f;
CubeMap cubeMap;
Model *cubeMapModel;
const float cubeMapScale = 4.0f;
//reflect refract shader variables
float eta[3];
float reflectionFactor;
float mixRRfactor;

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
	TwAddVarRW(myBar, "YRotation", TW_TYPE_FLOAT, &rotDelta, "");
	TwAddVarRW(myBar, "Eta", TW_TYPE_COLOR3F, &eta, "");
	TwAddVarRW(myBar, "Fresnel Power", TW_TYPE_FLOAT, &reflectionFactor, "");
    TwAddVarRW(myBar, "Light Direction", TW_TYPE_DIR3F, &lightDirection, " label='Light direction' opened=true help='Change the light direction.' ");
	TwAddVarRW(myBar, "Light Intensity", TW_TYPE_COLOR3F, &lightIntensity, "");
	TwAddButton(myBar, "Change model", changeShaderTypeCallback, NULL, "");
	TwAddVarRW(myBar, "mix Transmittance Factor", TW_TYPE_FLOAT, &mixRRfactor, "step = 0.05, help = 'How much of the reflection/refraction do we want'");
	//TwAddButton(myBar, "Change Shader Type", changeShaderTypeCallback, NULL, NULL);

//--------------------- end tweak bar bars ------------------


	//string blinnPhongVariables1[10] = {"position","normal","m_pvm","m_viewModel","m_normal","l_dir","diffuse","ambient","specular","shininess"};
	//vector<string> blinnPhongVariables2(blinnPhongVariables1,blinnPhongVariables1 + sizeof(blinnPhongVariables1)/sizeof(string));

	reflectRefractShader = ShaderManager::get().getShaderProgram("shaders/reflectRefract2VS.glsl","shaders/reflectRefract2FS.glsl",ShaderType::REFLECTREFRACT2);
	textureCubeShader = ShaderManager::get().getShaderProgram("shaders/textureCubeVS.glsl","shaders/textureCubeFS.glsl",ShaderType::TEXTURECUBE);
	normalMapShader = ShaderManager::get().getShaderProgram("shaders/normalMapVS.glsl","shaders/normalMapFS.glsl",ShaderType::NORMALMAP);
	reflectRefractNormalMapShader = ShaderManager::get().getShaderProgram("shaders/reflectRefractNormalMapVS.glsl","shaders/reflectRefractNormalMapFS.glsl",ShaderType::REFLECTREFRACTNORMALMAP);

	model1 = new Model("model1", "models/nanosuit3/exported/nanosuit.dae", reflectRefractNormalMapShader);
	model1Transform = SQTTransform(glm::vec3(-1,-0.3,-2),glm::vec3(0.07),glm::vec3(0));
	model2 = new Model("model2", "models/cubeNormal/cubeNormal.dae", reflectRefractNormalMapShader);
	model2Transform = SQTTransform(glm::vec3(-1,0,-1.9),glm::vec3(0.15),glm::vec3(0));
	//viewMatrix = glm::lookAt(glm::vec3(0,0,3.0f),glm::vec3(0),glm::vec3(0,1.0f,0));
	viewMatrix = camera.GetViewMatrix();
	projectionMatrix = glm::perspective(45.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	cubeMap.create(cubeMapScale, "models/cube/cube.obj", normalMapShader, "","models/Sorsele3/", ".jpg");

	eta[0] = (float)100/255;
	eta[1] = (float)90/255;
	eta[2] = (float)80/255;
	reflectionFactor = 5.0f;
	lightIntensity[0] = 1.0f;
	lightIntensity[1] = 1.0f;
	lightIntensity[2] = 1.0f;

	mixRRfactor = 0.5f;

}

void updateModels()
{
	lightDirection = glm::normalize(lightDirection);

	double delta = double(Timer::get().getLastInterval()) / 1000.0;
	model1Transform.pivotOnLocalAxisDegrees(0,rotDelta*delta, 0);
	model2Transform.pivotOnLocalAxisDegrees(0,rotDelta*delta, 0);
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
	//glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	viewMatrix = camera.GetViewMatrix();
	model1->renderCubeMap(textureCubeShader, cubeMap, model1Transform.getMatrix(), viewMatrix, projectionMatrix);
	if(model1question)
	{
		model1->renderNormalMap(normalMapShader, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, true, false);
		model1Transform.translateGlobal(0.7,0,0);
		model1->renderNormalMap(normalMapShader, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, false, true);
		model1Transform.translateGlobal(0.7,0,0);
		model1->renderNormalMap(normalMapShader, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, true, true);
		model1Transform.translateGlobal(0.7,0,0);
		model1->renderReflectRefractNormalMap(reflectRefractNormalMapShader, cubeMap, model1Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, eta, reflectionFactor, lightIntensity, mixRRfactor);
		model1Transform.translateGlobal(-2.1,0,0);

	}
	else
	{
		model2->renderNormalMap(normalMapShader, model2Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, true, false);
		model2Transform.translateGlobal(0.7,0,0);
		model2->renderNormalMap(normalMapShader, model2Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, false, true);
		model2Transform.translateGlobal(0.7,0,0);
		model2->renderNormalMap(normalMapShader, model2Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, lightIntensity, true, true);
		model2Transform.translateGlobal(0.7,0,0);
		model2->renderReflectRefractNormalMap(reflectRefractNormalMapShader, cubeMap, model2Transform.getMatrix(), viewMatrix, projectionMatrix, -lightDirection, eta, reflectionFactor, lightIntensity, mixRRfactor);
		model2Transform.translateGlobal(-2.1,0,0);

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
	model1question = !model1question;
}

#pragma endregion