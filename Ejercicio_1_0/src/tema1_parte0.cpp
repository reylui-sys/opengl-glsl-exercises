// Ejercicios tema 1 - parte 0 (PTG - 2026)
#include <iostream>
#include <GL/glut.h>

// Prototipos de las funciones
void drawBox();
GLboolean initOpenGL();
void display();
void resize(GLint, GLint);
void inactivo();
void keyboard(GLubyte, GLint, GLint);
void clickRaton(GLint, GLint, GLint, GLint);
void movimientoRaton(GLint, GLint);
void printFPS();

// Variables globales (estados globales de la aplicación)

GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = true;
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;			// Ancho inicial de la ventana
GLint g_Height = 500;			// Altura incial de la ventana


GLint main(GLint argc, char *argv[])
{
	glutInit(&argc, (char**)argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Ejercicios: tema 1 - parte 0");
	initOpenGL();
	glutDisplayFunc(display); // Registra el callback de la función display para gestionar el renderizado
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutMouseFunc(clickRaton); // Registra el callback de la función clickRaton para gestionar los clicks del ratón
	glutMotionFunc(movimientoRaton); // Registra el callback de la función movimientoRaton para gestionar el movimiento del ratón
	glutIdleFunc(inactivo); // Registra el callback de la función inactivo (idle) para que se mueva el cubo cuando no hay eventos

	glutMainLoop();
 
	return EXIT_SUCCESS;
}
 
void drawBox()
{
	glBegin(GL_QUADS);
 
		glColor3f(1.0f, 0.0f, 0.0f);
		// FRONT
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);
		glVertex3f( 0.5f,  0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f,  0.5f);
		// BACK
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f, -0.5f);
 
		glColor3f(0.0f, 1.0f, 0.0f);
		// LEFT
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f,  0.5f);
		glVertex3f(-0.5f,  0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		// RIGHT
		glVertex3f( 0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f, -0.5f);
		glVertex3f( 0.5f,  0.5f,  0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);
 
		glColor3f(0.0f, 0.0f, 1.0f);
		// TOP
		glVertex3f(-0.5f,  0.5f,  0.5f);
		glVertex3f( 0.5f,  0.5f,  0.5f);
		glVertex3f( 0.5f,  0.5f, -0.5f);
		glVertex3f(-0.5f,  0.5f, -0.5f);
		// BOTTOM
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f, -0.5f);
		glVertex3f( 0.5f, -0.5f,  0.5f);
	glEnd();
	return;
}
 
GLboolean initOpenGL()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
	glEnable(GL_DEPTH_TEST); // Habilita la funcionalidad del z-buffer. Compara el vaalor de profundidad de cada pixel para determinar si se debe dibujar o no
	// glDepthFunc(GL_LESS); // Tipo de prueba de profundidad a usar, es la que esta por defecto, por eso la comento
	
	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
 
	gluLookAt(
	0.0f, 0.0f, 3.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);
 
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
 
	drawBox(); // Se descomenta para dibujar el cubo
 
	printFPS();

	glutSwapBuffers();
	return;
}
 
void resize(GLint w, GLint h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
 
	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}
 
void inactivo()
{
	if (!mouseDown && animation)
	{
		xrot += 0.050f; // 0.3f
		yrot += 0.065f; // 0.4f
	}
 
	glutPostRedisplay();
	return;
}
 
void keyboard(GLubyte key, GLint x, GLint y)
{
	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(EXIT_SUCCESS); 
		break;
	case 'a': case 'A':
		animation = !animation;
	}
	return;
}
 
void clickRaton(GLint button, GLint state, GLint x, GLint y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;
 
		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
	return;
}
 
void movimientoRaton(GLint x, GLint y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;
 
		glutPostRedisplay();
	}
	return;
}


void printFPS()
{
	static GLint frameCount = 0;			//  Número de frames
	static GLfloat fps = 0;
	static GLint currentTime = 0, previousTime = 0;

	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);

	GLint timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);

		previousTime = currentTime;

		frameCount = 0;

		std::cout.precision(4);
		std::cout << "FPS: " << fps << std::endl;
	}
	return;
}
