#pragma once

#include "engine.h"

using namespace engine;
using math::Quaternion;

#define CAPTION "Snooker on Fire - Stage 2"

#define MAX_ACC 0.05

int WinX = 640, WinY = 480;
int WindowHandle = 0;
unsigned int FrameCount = 0;

GLfloat lastX = (GLfloat)(WinX / 2);
GLfloat lastY = (GLfloat)(WinY / 2);
GLfloat currentX, currentY;

GLuint UBO_BP = 0;

bool leftMouseButtonPressed = false;
bool rightMouseButtonPressed = false;

float animationDuration = 2.0f;
bool isSepia = false;
float interpolationFactor = 1.0f / animationDuration;
float currentInterpolation = 0.0f;
bool  interpolationSwitch = false;

float cueDrawDistance =   0.0f;
float        cuePitch = -18.0f;
float          cueYaw =   0.0f;
bool cueTakingTheShot =  false;
bool      cueReleased =  false;

float        whiteBallAcceleration = 0.0f;
Vector2 savedWhiteBallAcceleration;

float maxSpeed = 1.0f;
float maxRotationSpeed = 3.0f;
float distance = 15;
float distanceStep = 1.5f;

GLfloat deltaTime = 0.0f;
GLfloat oldTime = 0.0f;

Quaternion rotationQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
std::string activeSceneGraph = "snooker";

SceneNode* root;
SceneNode *ground;
SceneNode *whiteBall;
SceneNode *cue;
std::array<SceneNode*, 4> walls;
std::array<SceneNode*, 6> holes;
std::array<SceneNode*, 15> balls;

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

/////////////////////////////////////////////////////////////////////// FRAMEBUFFER

// FRAMEBUFFER
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

	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, WinX, WinY, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else // Using both a stencil and depth test, needs special format arguments
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WinX, WinY, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint framebuffer;
GLuint textureColorbuffer;

void createFrameBuffer() {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Create a color attachment texture
	textureColorbuffer = generateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WinX, WinY); // Use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
																								  // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	ShaderProgram *program2 = new ShaderProgram();
	// FRAMEBUFFER
	program2->compileShaderFromFile("data/shaders/framebuffers_screen.vert", ShaderType::VERTEX);
	program2->compileShaderFromFile("data/shaders/framebuffers_screen.frag", ShaderType::FRAGMENT);

	program2->bindAttribLocation(VERTICES, "position");
	program2->bindAttribLocation(TEXCOORDS, "texCoords");

	program2->link();
	ShaderProgramManager::instance()->add("screen", program2);

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
	std::string quadStr("data/meshes/tamSquare.obj");
	std::string ballStr("data/meshes/sphere.obj");
	std::string cylinderStr("data/meshes/cylinder.obj");

	Mesh *square = new Mesh(tamSquare);
	Mesh *ball = new Mesh(ballStr);
	Mesh *cylinder = new Mesh(cylinderStr);
	Mesh *quad = new Mesh(quadStr);

	square->create();
	ball->create();
	cylinder->create();
	quad->create();


	MeshManager::instance()->add("square", square);
	MeshManager::instance()->add("ball", ball);
	MeshManager::instance()->add("cylinder", cylinder);
	MeshManager::instance()->add("quad", quad);

	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects() {
}

void createSnooker() {
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
	std::string textTable("data/textures/tableTexture.bmp");

	Mesh* squareMesh = MeshManager::instance()->get("square");
	Mesh* ballMesh = MeshManager::instance()->get("ball");
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
	walls[0]->setModelMatrix(math::translate(Vector3(0.0f, 1.3f, 2.0f)) *
		math::scale(Vector3(0.82f, 0.1f, 1.0)));
	walls[0]->setTexture(textWood);

	walls[1] = ground->createNode("wall");
	walls[1]->setMesh(squareMesh);
	walls[1]->setModelMatrix(math::translate(Vector3(0.0f, -0.5f, 2.0f)) *
		math::scale(Vector3(0.82f, 0.1f, 1.0)));
	walls[1]->setTexture(textWood);

	walls[2] = ground->createNode("wall");
	walls[2]->setMesh(squareMesh);
	walls[2]->setModelMatrix(math::translate(Vector3(0.85f, 0.0f, 2.0f)) *
		math::scale(Vector3(0.1f, 1.0f, 1.0)));
	walls[2]->setTexture(textWood);

	walls[3] = ground->createNode("wall");
	walls[3]->setMesh(squareMesh);
	walls[3]->setModelMatrix(math::translate(Vector3(-0.95f, 0.0f, 2.0f)) *
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
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[0]->setTexture(textBall1);

	balls[1] = scenegraph->createNode("ball");
	balls[1]->setMesh(ballMesh);
	balls[1]->setModelMatrix(math::translate(Vector3(-4.4f + xOffset, 3.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[1]->setTexture(textBall2);

	balls[2] = scenegraph->createNode("ball");
	balls[2]->setMesh(ballMesh);
	balls[2]->setModelMatrix(math::translate(Vector3(-4.4f + xOffset, 2.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[2]->setTexture(textBall3);

	balls[3] = scenegraph->createNode("ball");
	balls[3]->setMesh(ballMesh);
	balls[3]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 1.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[3]->setTexture(textBall4);

	balls[4] = scenegraph->createNode("ball");
	balls[4]->setMesh(ballMesh);
	balls[4]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[4]->setTexture(textBall5);

	balls[5] = scenegraph->createNode("ball");
	balls[5]->setMesh(ballMesh);
	balls[5]->setModelMatrix(math::translate(Vector3(-4.8f + xOffset, 3.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[5]->setTexture(textBall6);

	balls[6] = scenegraph->createNode("ball");
	balls[6]->setMesh(ballMesh);
	balls[6]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 1.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[6]->setTexture(textBall7);

	balls[7] = scenegraph->createNode("ball");
	balls[7]->setMesh(ballMesh);
	balls[7]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 2.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[7]->setTexture(textBall8);

	balls[8] = scenegraph->createNode("ball");
	balls[8]->setMesh(ballMesh);
	balls[8]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 3.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[8]->setTexture(textBall9);

	balls[9] = scenegraph->createNode("ball");
	balls[9]->setMesh(ballMesh);
	balls[9]->setModelMatrix(math::translate(Vector3(-5.2f + xOffset, 4.0f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[9]->setTexture(textBall10);

	balls[10] = scenegraph->createNode("ball");
	balls[10]->setMesh(ballMesh);
	balls[10]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 0.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[10]->setTexture(textBall11);

	balls[11] = scenegraph->createNode("ball");
	balls[11]->setMesh(ballMesh);
	balls[11]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 1.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[11]->setTexture(textBall12);

	balls[12] = scenegraph->createNode("ball");
	balls[12]->setMesh(ballMesh);
	balls[12]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 2.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[12]->setTexture(textBall13);

	balls[13] = scenegraph->createNode("ball");
	balls[13]->setMesh(ballMesh);
	balls[13]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 3.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[13]->setTexture(textBall14);

	balls[14] = scenegraph->createNode("ball");
	balls[14]->setMesh(ballMesh);
	balls[14]->setModelMatrix(math::translate(Vector3(-5.6f + xOffset, 4.5f + yOffset, 1.0f + zOffset)) *
		math::scale(Vector3(1.0f, 1.0f, 1.0f) * scaleFactor));
	balls[14]->setTexture(textBall15);

	cue = scenegraph->createNode("cue");
	cue->setMesh(squareMesh);
	cue->setModelMatrix(math::translate(Vector3(whiteBall->getModelMatrix().getElement(0, 3) + 3.0f, whiteBall->getModelMatrix().getElement(1, 3), whiteBall->getModelMatrix().getElement(2, 3))) *
						math::rotate(-10.0f,Vector3(0.0f,1.0f,0.0f)) *
						math::scale(Vector3(4.0f, 0.1f, 0.1f)));
	cue->setTexture(textWood);

	ModelsManager::instance()->add(new Object(whiteBall->getModelMatrix()
		* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15f, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "whiteBall");

	for (int i = 1; i < 16; i++) {
		ModelsManager::instance()->add(new Object(balls[i - 1]->getModelMatrix()
			* Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f)).toMatrix(), 0.15f, Vector2(0, 0), Vector2(0.0, 0), Vector2(0.000, 0), 1), "ball" + std::to_string(i));
	}

	SceneGraphManager::instance()->add(activeSceneGraph, scenegraph);
}

void applyWhiteBallMovement() {
	if (KeyBuffer::instance()->isKeyDown('a')) {
		Vector2 newDir = ModelsManager::instance()->get("whiteBall")->speed() + Vector2(-0.01f, 0.0f);
		ModelsManager::instance()->get("whiteBall")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('d')) {
		Vector2 newDir = ModelsManager::instance()->get("whiteBall")->speed() + Vector2(0.01f, 0.0f);
		ModelsManager::instance()->get("whiteBall")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('w')) {
		Vector2 newDir = ModelsManager::instance()->get("whiteBall")->speed() + Vector2(0.0f, 0.01f);
		ModelsManager::instance()->get("whiteBall")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('s')) {
		Vector2 newDir = ModelsManager::instance()->get("whiteBall")->speed() + Vector2(0.0f, -0.01f);
		ModelsManager::instance()->get("whiteBall")->setSpeed(newDir);
	}

	if (KeyBuffer::instance()->isKeyDown('z')) {
		cueYaw += 1.0f;
	}

	if (KeyBuffer::instance()->isKeyDown('c')) {
		cueYaw -= 1.0f;
	}

	if (KeyBuffer::instance()->isKeyDown(' ')) {
		cueTakingTheShot = true;
		if (whiteBallAcceleration < MAX_ACC) {
			whiteBallAcceleration += 0.0005f;
			std::cout << "Cue Acceleration: " << whiteBallAcceleration << std::endl;
		}
		else
			whiteBallAcceleration = (float)MAX_ACC;

	}

	if (!KeyBuffer::instance()->isKeyDown(' ')) {
		if (whiteBallAcceleration > 0.0f)
			whiteBallAcceleration -= 0.0005f;
		else
			whiteBallAcceleration = 0.0f;
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
	applyWhiteBallMovement();
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

void computeCueRotation() {
	if (currentX != lastX || currentY != lastY) {

		cuePitch += currentY - lastY;
		//cueYaw += currentX - lastX;

		if (cuePitch < -48.0f)
			cuePitch = -48.0f;
		if (cuePitch > -18.0f)
			cuePitch = -18.0f;

		//if (cueYaw >  89.0f)
		//	cueYaw =  89.0f;
		//if (cueYaw < -89.0f)
		//	cueYaw = -89.0f;

		lastX = currentX;
		lastY = currentY;
	}
}

void setViewProjectionMatrix() {
	Matrix4 translation = math::translate(Vector3(0.0f, 0.0f, -distance));
	Matrix4 rotation = rotationQuaternion.toMatrix();
	SceneGraphManager::instance()->get(activeSceneGraph)->getCamera()->setViewMatrix(translation * rotation);
}

void setLightningAndPost()
{
	ShaderProgramManager::instance()->get("default")->use();
	ShaderProgramManager::instance()->get("default")->setUniform("AmbientProduct", 0.0, 0.0, 0.0, 0.0);
	ShaderProgramManager::instance()->get("default")->setUniform("DiffuseProduct", 1.0, 1.0, 1.0, 0.0);
	ShaderProgramManager::instance()->get("default")->setUniform("SpecularProduct", 1.0, 1.0, 1.0, 1.0);
	ShaderProgramManager::instance()->get("default")->setUniform("LightPosition", 0.0, 0.0, 20.0, 0.0);
	ShaderProgramManager::instance()->get("default")->setUniform("isSepia", isSepia);
}

// FRAMEBUFFER
GLuint quadVAO, quadVBO;
GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							 // Positions   // TexCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

// FRAMEBUFFER
void setScreenVAO()
{
	// Setup screen VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void drawSceneGraph() {
	ModelsManager::instance()->updateObjects(0);
	whiteBall->setModelMatrix(ModelsManager::instance()->get("whiteBall")->modelMatrix());

	for (int i = 1; i < 16; i++)
		balls[i-1]->setModelMatrix(ModelsManager::instance()->get("ball" + std::to_string(i))->modelMatrix());

	Matrix4 whiteModel = ModelsManager::instance()->get("whiteBall")->modelMatrix();
	Matrix4 translation = math::translate(Vector3(whiteModel.getElement(0, 3), whiteModel.getElement(1, 3), whiteModel.getElement(2, 3)));

	if (cueTakingTheShot) 
	{
		if (!cueReleased) {
			if (cueDrawDistance < MAX_ACC*50)
				cueDrawDistance += 0.05f;
			else
				cueDrawDistance = MAX_ACC*50;
		}
		else
		{
			if (cueDrawDistance > 0.0f)
				cueDrawDistance -= 0.30f;
			else
			{
				ModelsManager::instance()->get("whiteBall")->setAcceleration(savedWhiteBallAcceleration);
				cueDrawDistance = 0.0f; 
				cueReleased = false;
				cueTakingTheShot = false;
			}
		}
	}

	cue->setModelMatrix(
		// TRANSLATE TO THE DESIRED POSITION (white ball location)
		   translation *
		// ROTATE AROUND Z (moves the cue left/right)
		   math::rotate(cueYaw, Vector3(0.0f, 0.0f, 1.0f)) *
		// ROTATE AROUND X (moves the cue up/down)
		   math::rotate(cuePitch, Vector3(0.0f, 1.0f, 0.0f)) *
		// TRANSLATE BASED ON ACCELERATION
		   math::translate(Vector3(cueDrawDistance, 0.0f, 0.0f)) *
		// TRANSLATE SO THE TIP IS AT THE ORIGIN
		   math::translate(Vector3(4.8f, 0.0f, 0.0f)) *
		// SCALE THE CUE
		   math::scale(Vector3(4.0f, 0.1f, 0.1f))
	);
	
	for (int i = 1; i < 16; i++)
		balls[i - 1]->setModelMatrix(ModelsManager::instance()->get("ball" + std::to_string(i))->modelMatrix());

	if (ModelsManager::instance()->shotInProgress())
		cue->setDrawable(false);
	else
		cue->setDrawable(true);

	SceneGraphManager::instance()->get(activeSceneGraph)->draw();
}

// FRAMEBUFFER
void drawBuffer() {

	/////////////////////////////////////////////////////
	// Bind to default framebuffer again and draw the 
	// quad plane with attched screen texture.
	// //////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

	// Draw Screen
	//screenShader.Use();
	ShaderProgramManager::instance()->get("screen")->use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// Use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void drawScene()
{
	computeTime();
	applyMotion();
	setViewProjectionMatrix();
	setLightningAndPost();
	drawSceneGraph();

	//// FRAMEBUFFER
	setScreenVAO();
	drawBuffer();

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

	// FRAMEBUFFER
	/////////////////////////////////////////////////////
	// Bind to framebuffer and draw to color texture 
	// as we normally would.
	// //////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clear all attached buffers        
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer so why bother with clearing?

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
	if (key == 'm') {
		if (Snapshot::instance()->takeSnapshotToBMP(WinX, WinY))
			std::cout << "Smile for the bmp camera!" << std::endl;
	}
	if (key == 'n') {

		if (Snapshot::instance()->takeSnapshotToTGA(WinX, WinY))
			std::cout << "Smile for the tga camera!" << std::endl;
	}
	if (key == 'k')
	{
		isSepia = !isSepia;
	}
}

void keyboardPressSpecial(int key, int x, int y) {
	KeyBuffer::instance()->pressSpecialKey(key);
}

void keyboardUp(unsigned char key, int x, int y) {
	KeyBuffer::instance()->releaseKey(key);
	if (key = ' ') {
		cueReleased = true;
		savedWhiteBallAcceleration = Vector2(-whiteBallAcceleration*cos(math::toRadians(cueYaw)), 
											 -whiteBallAcceleration*sin(math::toRadians(cueYaw))
											);
	}
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

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		rightMouseButtonPressed = true;
		lastX = currentX = (float)x;
		lastY = currentY = (float)y;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftMouseButtonPressed = false;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		rightMouseButtonPressed = false;

	if (button == MOUSE_SCROLL_UP)
		distance -= distanceStep;

	if (button == MOUSE_SCROLL_DOWN)
		distance += distanceStep;
}

void mouseMovement(int x, int y) {
	if (leftMouseButtonPressed) {
		currentX = (float)x;
		currentY = (float)y;
		computeAngleAxis();
	}
	if (rightMouseButtonPressed) {
		currentX = (float)x;
		currentY = (float)y;
		computeCueRotation();
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

	//FRAMEBUFFER
	createFrameBuffer();
	createSnooker();
}

int main(int argc, char* argv[])
{
	init(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
