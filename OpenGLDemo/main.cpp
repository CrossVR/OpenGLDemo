#include <gl/glew.h>
#include <windows.h>
#include <GL/glut.h>
#include "ObjModel.h"
#include "shader.h"
#include <limits>

#define _USE_MATH_DEFINES
#include <math.h>
#include <gmtl/Generate.h>

#define LEFT_EYE 0
#define RIGHT_EYE 1

int window_width = 1024;
int window_height = 768;

bool draggingLeftButton = false;
bool inFullscreen = false;
unsigned long last_idle_time;

int timer;
unsigned long lastTimer;

float rotation = 0;

ObjModel* model;
ShaderProgram* shader;

gmtl::Matrix44f		modelViewMatrix;
gmtl::Matrix44f		projectionMatrix[2];
gmtl::Matrix44f		modelViewProjectionMatrix[2];

int frameCount = 0;
unsigned long lastFrameRate;

GLuint color, depth, fbo;

void initFBO()
{
	glGenTextures(1, &color);
	glGenTextures(1, &depth);
	glGenFramebuffers(1, &fbo);

	glBindTexture(GL_TEXTURE_2D_ARRAY, color);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, window_width, window_height, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, window_width, window_height, 2, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);

	GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (e != GL_FRAMEBUFFER_COMPLETE)
		printf("There is a problem with the FBO\n");
}

void reshape(GLint width, GLint height)
{
	window_width = width;
	window_height = height;

	GLdouble fW, fH, zNear, eyeSep, eyeOff, focalLength;
	eyeSep = 0.65;
	zNear = 10.0;
	focalLength = 100.0;
	eyeOff = (eyeSep / 2.0) * (zNear / focalLength);
	fH = tan(65.0f / (360 * M_PI)) * zNear;
	fW = fH * ((float)width / height);

	projectionMatrix[LEFT_EYE] = gmtl::setFrustum<float>(projectionMatrix[LEFT_EYE], -fW + eyeOff, fH, fW + eyeOff, -fH, 1.0f, 1000.0f);
	projectionMatrix[RIGHT_EYE] = gmtl::setFrustum<float>(projectionMatrix[RIGHT_EYE], -fW - eyeOff, fH, fW - eyeOff, -fH, 1.0f, 1000.0f);

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &color);
	glDeleteTextures(1, &depth);
	initFBO();
}


void keyboard(unsigned char keyCode, int x, int y)
{
	if(keyCode == 27)
		exit(0);

	if (keyCode == 13)
		glutFullScreen();
}

void mouseButton(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
		draggingLeftButton = (state == GLUT_DOWN);
}

void mouseMotion(int x, int y)
{
	if(draggingLeftButton)
		glutPostRedisplay();
}


void initGL()
{
	glewInit();

	glEnable(GL_DEPTH_TEST);


//	model = new ObjModel("models/car/honda_jazz.obj");
	model = new ObjModel("models/ship/shipA_OBJ.obj");
//	model = new ObjModel("models/bloemetje/PrimroseP.obj");
//	shader = new ShaderProgram("minimal.vert", "minimal.frag");
//	shader = new ShaderProgram("lighting.vert", "lighting.frag");
//	shader = new ShaderProgram("specular.vert", "specular.frag");
//	shader = new ShaderProgram("toon.vert", "toon.frag");
	shader = new ShaderProgram("toon.vert", "toon.frag", "stereo.geom");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texCoord", 2);

	shader->bindFragLocation("fragColor", 0);
	shader->link();
	shader->use();

	shader->setUniformInt("s_texture", 0);
	glClearColor(0.0, 0.0, 0.1, 1.0);

	initFBO();
}

void display()
{
	// Clear frame buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set up viewing transformation, looking down -Z axis

	gmtl::identity(modelViewMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//camera
	modelViewMatrix *= gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0, 0, -75));
	modelViewMatrix *= gmtl::make<gmtl::Matrix44f>(gmtl::EulerAngleXYZf(0, rotation, rotation/10.0));

	gmtl::Matrix33f normalMatrix;
	normalMatrix.set(modelViewMatrix[0][0], modelViewMatrix[0][1], modelViewMatrix[0][2],
					 modelViewMatrix[1][0], modelViewMatrix[1][1], modelViewMatrix[1][2],
					 modelViewMatrix[2][0], modelViewMatrix[2][1], modelViewMatrix[2][2]);
	normalMatrix = gmtl::transpose(gmtl::invert(normalMatrix));
	//end of camera

	modelViewProjectionMatrix[LEFT_EYE] = projectionMatrix[LEFT_EYE] * modelViewMatrix;
	modelViewProjectionMatrix[RIGHT_EYE] = projectionMatrix[RIGHT_EYE] * modelViewMatrix;
	
	shader->setUniformInt("timer", timer);
	shader->setUniformMatrix3("normalMatrix", normalMatrix);
	shader->setUniformMatrix4("modelViewProjectionMatrix", modelViewProjectionMatrix, 2);
	shader->setUniformMatrix4("modelViewMatrix", modelViewMatrix);

	model->draw();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, window_width / 2, window_height);
	glBindImageTexture(GL_TEXTURE_2D, color, 0, GL_TRUE, LEFT_EYE, GL_READ_ONLY, GL_RGBA);
	glBegin(GL_QUADS);
	glVertex3f(-10.0f, 10.0f, 0.0f);
	glVertex3f(10.0f, 10.0f, 0.0f);
	glVertex3f(10.0f, -10.0f, 0.0f);
	glVertex3f(-10.0f, -10.0f, 0.0f);
	glEnd();

	glViewport(window_width / 2, 0, window_width / 2, window_height);
	glBindImageTexture(GL_TEXTURE_2D, color, 0, GL_TRUE, RIGHT_EYE, GL_READ_ONLY, GL_RGBA);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();

	glutSwapBuffers();
	frameCount++;
}

void update()
{
	int diff = GetTickCount() - last_idle_time;
	rotation += 0.001*diff;

	glutPostRedisplay();
	last_idle_time = GetTickCount();

	if(last_idle_time - lastTimer > 10)
	{
		timer++;
		lastTimer = last_idle_time;
	}
	if(last_idle_time - lastFrameRate > 5000)
	{
		std::cout<<"FPS: "<<frameCount / 5<<std::endl;
		frameCount = 0;
		lastFrameRate = last_idle_time;
	}
}


int main(int argc, char** argv)
{
  // GLUT Window Initialization:
  glutInit (&argc, argv);
  glutInitWindowSize (window_width, window_height);
  glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow ("OpenGL Shader Demo");
  // Initialize OpenGL graphics state
  initGL();
  // Register callbacks:
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutMouseFunc (mouseButton);
  glutMotionFunc (mouseMotion);
  glutIdleFunc (update);

  last_idle_time = GetTickCount();
  lastFrameRate = last_idle_time;
  timer = 0;
  glutMainLoop ();
  return 0;
}