#pragma once

#include "engine.h"

using namespace engine;
using math::Quaternion;

#define CAPTION "Snooker on Fire - Stage 2"

int WinX = 640, WinY = 480;
int WindowHandle = 0;
unsigned int FrameCount = 0;

GLfloat lastX = (GLfloat)(WinX / 2);
GLfloat lastY = (GLfloat)(WinY / 2);
GLfloat currentX, currentY;

GLuint UBO_BP = 0;

bool leftMouseButtonPressed = false;

float groundY, groundZ, groundX = 0.0f;
float groundRot = 0.0f;
float distance = 15;
float tamHeight = 7.0f;

float animationDuration = 2.0f;
float interpolationFactor = 1.0f / animationDuration;
float currentInterpolation = 0.0f;
bool  interpolationSwitch = false;

float cueAcceleration = 0.0f;

float maxSpeed = 1.0f;
float maxRotationSpeed = 3.0f;
float distanceStep = 1.5f;

GLfloat deltaTime = 0.0f;
GLfloat oldTime = 0.0f;

Quaternion rotationQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

std::string activeSceneGraph = "squirrel";

/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if (isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs

void createShaderProgram()
{
	ShaderProgram *program = new ShaderProgram();

	program->compileShaderFromFile("data/shaders/vertexShaderTextures.vert", ShaderType::VERTEX);

	program->compileShaderFromFile("data/shaders/fragmentShaderTextures.frag", ShaderType::FRAGMENT);

	program->bindAttribLocation(VERTICES, "in_Position");
	program->bindAttribLocation(TEXCOORDS, "in_TexCoords");
	program->bindAttribLocation(NORMALS, "in_Normal");
	program->addUniformBlock("Camera", UBO_BP);

	program->link();

	ShaderProgramManager::instance()->add("default", program);

	//checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);

	//checkOpenGLError("ERROR: Could not destroy shaders.");
}

void createMeshes()
{
	std::string tamSquare("data/meshes/cube.obj");
	std::string ballStr("data/meshes/sphere.obj");
	std::string cylinderStr("data/meshes/cylinder.obj");

	Mesh *square = new Mesh(tamSquare);
	Mesh *ball = new Mesh(ballStr);
	Mesh *cylinder = new Mesh(cylinderStr);

	square->create();
	ball->create();
	cylinder->create();


	MeshManager::instance()->add("square", square);
	MeshManager::instance()->add("ball", ball);
	MeshManager::instance()->add("cylinder", cylinder);

	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}


SceneNode* root;
SceneNode *ground;
SceneNode *whiteBall;
SceneNode *cue;
std::array<SceneNode*,4> walls;
std::array<SceneNode*,6> holes;
std::array<SceneNode*,15> balls;

void destroyBufferObjects() {
}

void createSquirrel() {
	SceneGraph* scenegraph = new SceneGraph();
	scenegraph->setCamera(new ArcballCamera(UBO_BP));

	scenegraph->getCamera()->setProjectionMatrix(
		math::Perspective(30.0f, (float)WinX / WinY, 0.1f, 100.0f));

	//Textures definition
	std::string textBrick("data/textures/bricks.bmp");
	std::string textRed("data/textures/red.bmp");
	std::string textBlue("data/textures/blue.bmp");
	std::string textGreen("data/textures/green.bmp");
	std::string textBrown("data/textures/brown.bmp");
	std::string textBlack("data/textures/black.bmp");
	std::string textWhite("data/textures/white.bmp");
	std::string textYellow("data/textures/yellow.bmp");
	std::string textBall1("data/textures/ball1.bmp");
	std::string textBall2("data/textures/ball2.bmp");
	std::string textBall3("data/textures/ball3.bmp");
	std::string textBall4("data/textures/ball4.bmp");
	std::string textBall5("data/textures/ball5.bmp");
	std::string textBall6("data/textures/ball6.bmp");
	std::string textBall7("data/textures/ball7.bmp");
	std::string textBall8("data/textures/ball8.bmp");
	std::string textBall9("data/textures/ball9.bmp");
	std::string textBall10("data/textures/ball10.bmp");
	std::string textBall11("data/textures/ball11.bmp");
	std::string textBall12("data/textures/ball12.bmp");
	std::string textBall13("data/textures/ball13.bmp");
	std::string textBall14("data/textures/ball14.bmp");
	std::string textBall15("data/textures/ball15.bmp");
	std::string textWood("data/textures/wood1.bmp");
	//std::string textTable("data/textures/tableTexture.bmp");
	std::string textTable("data/textures/tableTexture2.bmp");

	Mesh* squareMesh   = MeshManager::instance()->get("square");
	Mesh* ballMesh     = MeshManager::instance()->get("ball");
	Mesh* cylinderMesh = MeshManager::instance()->get("cylinder");

	root = scenegraph->getRoot();
	root->setShaderProgram(ShaderProgramManager::instance()->get("default"));

	ground = scenegraph->createNode("ground");
	ground->setMesh(squareMesh);
	ground->setModelMatrix(math::translate(Vector3(0.0f, 0.0f, 0.0f)) *
		Quaternion(0.0f, Vector3(-1.0f, 0.0f, 0.0f)).toMatrix() *
		math::scale(Vector3(10.0f, 6.0f, 0.6f)));

	ground->setTexture(textTable);

	walls[0] = ground->createNode("wall");
	walls[0]->setMesh(squareMesh);
	walls[0]->setModelMatrix(math::translate(Vector3(0.0f, 1.3, 2.0f)) *
							math::scale(Vector3(0.82f, 0.1f, 1.0)));
	walls[0]->setTexture(textWood);

	walls[1] = ground->createNode("wall");
	walls[1]->setMesh(squareMesh);
	walls[1]->setModelMatrix(math::translate(Vector3(0.0f, -0.5, 2.0f)) *
		   				  math::scale(Vector3(0.82f, 0.1f, 1.0)));
	walls[1]->setTexture(textWood);

	walls[2] = ground->createNode("wall");
	walls[2]->setMesh(squareMesh);
	walls[2]->setModelMatrix(math::translate(Vector3(0.85f, 0.0, 2.0f)) *
		  				  math::scale(Vector3(0.1f, 1.0f, 1.0)));
	walls[2]->setTexture(textWood);

	walls[3] = ground->createNode("wall");
	walls[3]->setMesh(squareMesh);
	walls[3]->setModelMatrix(math::translate(Vector3(-0.95f, 0.0, 2.0f)) *
		 				  math::scale(Vector3(0.1f, 1.0f, 1.0)));
	walls[3]->setTexture(textWood);

	float holeSize = 0.1f;
	holes[0] = ground->createNode("hole");
	holes[0]->setMesh(ballMesh);
	holes[0]->setModelMatrix(math::translate(Vector3(0.7f, 1.2f, 0.8f)) *
							 (math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[0]->setTexture(textBlack);

	holes[1] = ground->createNode("hole");
	holes[1]->setMesh(ballMesh);
	holes[1]->setModelMatrix(math::translate(Vector3(0.0f, 1.2f, 0.8f)) *
		   				 (math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[1]->setTexture(textBlack);

	holes[2] = ground->createNode("hole");
	holes[2]->setMesh(ballMesh);
	holes[2]->setModelMatrix(math::translate(Vector3(-0.82f, 1.2f, 0.8f)) *
		  				 (math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[2]->setTexture(textBlack);

	holes[3] = ground->createNode("hole");
	holes[3]->setMesh(ballMesh);
	holes[3]->setModelMatrix(math::translate(Vector3(-0.82f, -0.3f, 0.8f)) *
		  		        	(math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[3]->setTexture(textBlack);

	holes[4] = ground->createNode("hole");
	holes[4]->setMesh(ballMesh);
	holes[4]->setModelMatrix(math::translate(Vector3(0.0f, -0.3f, 0.8f)) *
		    	        	(math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[4]->setTexture(textBlack);

	holes[5] = ground->createNode("hole");
	holes[5]->setMesh(ballMesh);
	holes[5]->setModelMatrix(math::translate(Vector3(0.7f, -0.3f, 0.8f)) *
		    	        	(math::scale(Vector3(0.05f, 0.1f, 0.0)) * holeSize));
	holes[5]->setTexture(textBlack);

	float xOffset = 0.0f, yOffset = 0.0f, zOffset = -0.2f;
	float scaleFactor = 0.3f;

	whiteBall = scenegraph->createNode("ball");
	whiteBall->setMesh(ballMesh);
	whiteBall->setModelMatrix(math::translate(Vector3(5.0f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
		 				  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	whiteBall->setTexture(textWhite);

	balls[0] = scenegraph->createNode("ball");
	balls[0]->setMesh(ballMesh);
	balls[0]->setModelMatrix(math::translate(Vector3(-4.0f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
						  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		 				  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[0]->setTexture(textBall1);

	balls[1] = scenegraph->createNode("ball");
	balls[1]->setMesh(ballMesh);
	balls[1]->setModelMatrix(math::translate(Vector3(-4.4f + xOffset, 3.0f + yOffset, 1.0f + zOffset)) *
					  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		   			  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[1]->setTexture(textBall2);

	balls[2] = scenegraph->createNode("ball");
	balls[2]->setMesh(ballMesh);
	balls[2]->setModelMatrix(math::translate(Vector3(-4.4f + xOffset, 2.0f + yOffset, 1.0f + zOffset)) *
					  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		  			  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[2]->setTexture(textBall3);

	balls[3] = scenegraph->createNode("ball");
	balls[3]->setMesh(ballMesh);
	balls[3]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 1.5f + yOffset, 1.0f + zOffset)) *
				  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		   		  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[3]->setTexture(textBall4);

	balls[4] = scenegraph->createNode("ball");
	balls[4]->setMesh(ballMesh);
	balls[4]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
				  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		   		  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[4]->setTexture(textBall5);

	balls[5] = scenegraph->createNode("ball");
	balls[5]->setMesh(ballMesh);
	balls[5]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 3.5f + yOffset, 1.0f + zOffset)) *
				  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		   		  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[5]->setTexture(textBall6);

	balls[6] = scenegraph->createNode("ball");
	balls[6]->setMesh(ballMesh);
	balls[6]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 1.0f + yOffset, 1.0f + zOffset)) *
		    	  //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		  		  math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[6]->setTexture(textBall7);

	balls[7] = scenegraph->createNode("ball");
	balls[7]->setMesh(ballMesh);
	balls[7]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 2.0f + yOffset, 1.0f + zOffset)) *
		   		 //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
				 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[7]->setTexture(textBall8);

	balls[8] = scenegraph->createNode("ball");
	balls[8]->setMesh(ballMesh);
	balls[8]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 3.0f + yOffset, 1.0f + zOffset)) *
		  		 //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		    	 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[8]->setTexture(textBall9);

	balls[9] = scenegraph->createNode("ball");
	balls[9]->setMesh(ballMesh);
	balls[9]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 4.0f + yOffset, 1.0f + zOffset)) *
		    	 //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		    	 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[9]->setTexture(textBall10);

	balls[10] = scenegraph->createNode("ball");
	balls[10]->setMesh(ballMesh);
	balls[10]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 0.5f + yOffset, 1.0f + zOffset)) *
		    	 //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		    	 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[10]->setTexture(textBall11);
	
	balls[11] = scenegraph->createNode("ball");
	balls[11]->setMesh(ballMesh);
	balls[11]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 1.5f + yOffset, 1.0f + zOffset)) *
		         //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		    	 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[11]->setTexture(textBall12);

	balls[12] = scenegraph->createNode("ball");
	balls[12]->setMesh(ballMesh);
	balls[12]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
		   	 //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		   	 math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[12]->setTexture(textBall13);

	balls[13] = scenegraph->createNode("ball");
	balls[13]->setMesh(ballMesh);
	balls[13]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 3.5f + yOffset, 1.0f + zOffset)) *
		     //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		     math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[13]->setTexture(textBall14);

	balls[14] = scenegraph->createNode("ball");
	balls[14]->setMesh(ballMesh);
	balls[14]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 4.5f + yOffset, 1.0f + zOffset)) *
		     //math::rotate(200.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(-60.0f,Vector3(0.0f,0.0f,1.0f)) *
		     math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[14]->setTexture(textBall15);


	cue = scenegraph->createNode("cue");
	cue->setMesh(squareMesh);
	cue->setModelMatrix(math::translate(Vector3(10.0f + xOffset, 2.5f + yOffset, 2.0f + zOffset)) *
						math::rotate(-10.0f,Vector3(0.0f,1.0f,0.0f)) *
						math::scale(Vector3(4.0f, 0.1f, 0.1f)));
	cue->setTexture(textWood);

	//square1 = ground->createNode("square1");
	//square1->setMesh(squareMesh);
	//square1->setModelMatrix(math::translate(Vector3(-1.0f, 0.0f, 2.0f)) 
	//	* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix());

	ModelsManager::instance()->add(new Object(whiteBall->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2( 0,0 ), Vector2( 0.0,0 ), Vector2( 0.000,0 ), 1), "cue");

	ModelsManager::instance()->add(new Object(balls[0]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball1");

	ModelsManager::instance()->add(new Object(balls[1]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball2");

	ModelsManager::instance()->add(new Object(balls[2]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball3");

	ModelsManager::instance()->add(new Object(balls[3]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0, 0), 1), "ball4");

	ModelsManager::instance()->add(new Object(balls[4]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball5");

	ModelsManager::instance()->add(new Object(balls[5]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball6");


	ModelsManager::instance()->add(new Object(balls[6]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball7");


	ModelsManager::instance()->add(new Object(balls[7]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball8");


	ModelsManager::instance()->add(new Object(balls[8]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball9");

	ModelsManager::instance()->add(new Object(balls[9]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball10");

	ModelsManager::instance()->add(new Object(balls[10]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball11");

	ModelsManager::instance()->add(new Object(balls[11]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball12");

	ModelsManager::instance()->add(new Object(balls[12]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball13");

	ModelsManager::instance()->add(new Object(balls[13]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball14");

	ModelsManager::instance()->add(new Object(balls[14]->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball15");

	//ModelsManager::instance()->add(new Object(math::translate(Vector3(-1.0f, 0.0f, 2.0f))
	//	* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 2, Vector2( 0,0 ), Vector2( 0.01,0 ), Vector2( -0.00001,0 )), "square1");

	//square1->setTexture(textBrick);

	SceneGraphManager::instance()->add(activeSceneGraph, scenegraph);
}

void applyGroundMovement() {
	float angleStep = 0.05f * deltaTime;
	float moveStep = 0.0025f * deltaTime;

	if (KeyBuffer::instance()->isSpecialKeyDown(GLUT_KEY_UP) && groundY < maxSpeed)
		groundY += moveStep;
	else if (groundY > 0.0f)
		groundY -= moveStep;

	if (KeyBuffer::instance()->isSpecialKeyDown(GLUT_KEY_DOWN) && groundY > -maxSpeed)
		groundY -= moveStep;
	else if (groundY < 0.0f)
		groundY += moveStep;

	if (KeyBuffer::instance()->isSpecialKeyDown(GLUT_KEY_LEFT) && groundX > -maxSpeed)
		groundX -= moveStep;
	else if (groundX < 0.0f)
		groundX += moveStep;

	if (KeyBuffer::instance()->isSpecialKeyDown(GLUT_KEY_RIGHT) && groundX < maxSpeed)
		groundX += moveStep;
	else if (groundX > 0.0f)
		groundX -= moveStep;

	if (KeyBuffer::instance()->isKeyDown('q') && groundRot > -maxSpeed)
		groundRot -= angleStep;
	else if (groundRot < 0.0f)
		groundRot += angleStep;

	if (KeyBuffer::instance()->isKeyDown('e') && groundRot < maxSpeed)
		groundRot += angleStep;
	else if (groundRot > 0.0f)
		groundRot -= angleStep;

	if (KeyBuffer::instance()->isKeyDown('a')) {
		Vector2 newDir = ModelsManager::instance()->get("cue")->speed() + Vector2(-0.01f, 0.0f);
		ModelsManager::instance()->get("cue")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('d')) {
		Vector2 newDir = ModelsManager::instance()->get("cue")->speed() + Vector2(0.01f, 0.0f);
		ModelsManager::instance()->get("cue")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('w')) {
		Vector2 newDir = ModelsManager::instance()->get("cue")->speed() + Vector2(0.0f, 0.01f);
		ModelsManager::instance()->get("cue")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('s')) {
		Vector2 newDir = ModelsManager::instance()->get("cue")->speed() + Vector2(0.0f, -0.01f);
		ModelsManager::instance()->get("cue")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown(' ')) {
		cueAcceleration += 0.0005f;
		std::cout << cueAcceleration << std::endl;
	}

	if (!KeyBuffer::instance()->isKeyDown(' ')) {
		if(cueAcceleration > 0.0f)
			cueAcceleration -= 0.0005f;
	}


}

void computeInterpolation() {
	float interpolationStep = interpolationFactor * deltaTime;

	if (interpolationSwitch)
		currentInterpolation += interpolationStep;
	else
		currentInterpolation -= interpolationStep;

	if (currentInterpolation < 0.0f)
		currentInterpolation = 0.0f;

	if (currentInterpolation > 1.0f)
		currentInterpolation = 1.0f;
}

void applyMotion() {

	applyGroundMovement();

	computeInterpolation();
}

void computeTime() {
	GLfloat currentTime = ((GLfloat)glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	deltaTime = currentTime - oldTime;
	oldTime = currentTime;
}

void computeAngleAxis() {
	if (currentX != lastX || currentY != lastY) {
		float rotX = currentX - lastX;
		float rotY = currentY - lastY;
		Quaternion rotationQtrnY = Quaternion(rotX, math::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		Quaternion rotationQtrnX = Quaternion(rotY, math::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		rotationQuaternion = rotationQtrnX * rotationQtrnY * rotationQuaternion;
		lastX = currentX;
		lastY = currentY;
	}
}

void setViewProjectionMatrix() {
	Matrix4 translation = math::translate(Vector3(0.0f, 0.0f, -distance));
	Matrix4 rotation = rotationQuaternion.toMatrix();
	SceneGraphManager::instance()->get(activeSceneGraph)->getCamera()->setViewMatrix(translation * rotation);
}

void drawSceneGraph() {
	//ground->applyMatrix(math::translate(Vector3(groundX, groundY, groundZ)) *
	//	math::rotate(groundRot, Vector3(-1.0f, 0.0f, 0.0f)));
	if (!KeyBuffer::instance()->isKeyDown(' ') && cueAcceleration > 0.0f) {
		std::cout << "o bagulho é loco mano" << std::endl;
	}

	ModelsManager::instance()->updateObjects(0);
	whiteBall->setModelMatrix(ModelsManager::instance()->get("cue")->modelMatrix());
	balls[0]->setModelMatrix(ModelsManager::instance()->get("ball1")->modelMatrix());
	balls[1]->setModelMatrix(ModelsManager::instance()->get("ball2")->modelMatrix());
	balls[2]->setModelMatrix(ModelsManager::instance()->get("ball3")->modelMatrix());
	balls[3]->setModelMatrix(ModelsManager::instance()->get("ball4")->modelMatrix());
	balls[4]->setModelMatrix(ModelsManager::instance()->get("ball5")->modelMatrix());
	balls[5]->setModelMatrix(ModelsManager::instance()->get("ball6")->modelMatrix());
	balls[6]->setModelMatrix(ModelsManager::instance()->get("ball7")->modelMatrix());
	balls[7]->setModelMatrix(ModelsManager::instance()->get("ball8")->modelMatrix());
	balls[8]->setModelMatrix(ModelsManager::instance()->get("ball9")->modelMatrix());
	balls[9]->setModelMatrix(ModelsManager::instance()->get("ball10")->modelMatrix());
	balls[10]->setModelMatrix(ModelsManager::instance()->get("ball11")->modelMatrix());
	balls[11]->setModelMatrix(ModelsManager::instance()->get("ball12")->modelMatrix());
	balls[12]->setModelMatrix(ModelsManager::instance()->get("ball13")->modelMatrix());
	balls[13]->setModelMatrix(ModelsManager::instance()->get("ball14")->modelMatrix());
	balls[14]->setModelMatrix(ModelsManager::instance()->get("ball15")->modelMatrix());

	//balls[0]->setModelMatrix(math::translate(Vector3(-4.0f, 2.5f , 1.0f )) *
	//					  math::rotate(20.0f,Vector3(1.0f,0.0f,0.0f)) * math::rotate(tamHeight,Vector3(0.0f,0.0f,1.0f)) *
	//	 				  math::scale(Vector3(1.0f, 1.0f, 1.0f) * 0.3f));

	//square1->setModelMatrix(ModelsManager::instance()->get("square1")->model());

	//ground->applyMatrixToChildren(math::translate(Vector3(0.5f, 1.5f, 4.0f)) *
	//	math::rotate(90.0f, Vector3(1.0f, 0.0f, 0.0f)) *
	//	math::scale(Vector3(0.6f, 5.0f, 0.6f)));

	SceneGraphManager::instance()->get(activeSceneGraph)->draw();
}

void drawScene()
{
	computeTime();
	applyMotion();
	setViewProjectionMatrix();
	drawSceneGraph();

	glUseProgram(0);
	glBindVertexArray(0);

}

void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();
}

void display()
{
	++FrameCount;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	WinX = w;
	WinY = h;
	glViewport(0, 0, WinX, WinY);
}

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}

void keyboardPress(unsigned char key, int x, int y) {
	KeyBuffer::instance()->pressKey(key);
	if (key == 'g')
		interpolationSwitch = !interpolationSwitch;
}

void keyboardPressSpecial(int key, int x, int y) {
	KeyBuffer::instance()->pressSpecialKey(key);
}

void keyboardUp(unsigned char key, int x, int y) {
	KeyBuffer::instance()->releaseKey(key);
	if(key = ' ')
		ModelsManager::instance()->get("cue")->setAcceleration(Vector2(-cueAcceleration, 0.0f));
}

void keyboardUpSpecial(int key, int x, int y) {
	KeyBuffer::instance()->releaseSpecialKey(key);
}

void mousePress(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		leftMouseButtonPressed = true;
		lastX = currentX = (float)x;
		lastY = currentY = (float)y;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftMouseButtonPressed = false;

	if (button == MOUSE_SCROLL_UP) {
		distance -= distanceStep;
	}

	if (button == MOUSE_SCROLL_DOWN)
		distance += distanceStep;

}

void mouseMovement(int x, int y) {
	if (leftMouseButtonPressed) {
		currentX = (float)x;
		currentY = (float)y;
		computeAngleAxis();
	}
}

/////////////////////////////////////////////////////////////////////// SETUP

void setupCallbacks()
{
	glutCloseFunc(cleanup);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutKeyboardFunc(keyboardPress);
	glutSpecialFunc(keyboardPressSpecial);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialUpFunc(keyboardUpSpecial);
	glutMouseFunc(mousePress);
	glutMotionFunc(mouseMovement);
}

void checkOpenGLInfo()
{
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
	std::cerr << "OpenGL version " << version << std::endl;
	std::cerr << "GLSL version " << glslVersion << std::endl;
}

void setupOpenGL()
{
	checkOpenGLInfo();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0, 1.0);
	glClearDepth(1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void setupGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	GLenum err_code = glGetError();
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowSize(WinX, WinY);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	WindowHandle = glutCreateWindow(CAPTION);
	if (WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void init(int argc, char* argv[])
{
	setupGLUT(argc, argv);
	setupGLEW();
	setupOpenGL();
	setupCallbacks();

	createMeshes();

	createShaderProgram();

	createSquirrel();

}

int main(int argc, char* argv[])
{
	init(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
