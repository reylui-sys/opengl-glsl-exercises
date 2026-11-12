// Ejercicios tema 1 - parte 1 (PTG - 2026)
#include <iostream>
#include <cmath>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawBox();
void drawSolarSystem(); // Nueva función para dibujar el sistema solar
void projecTionMode();  // Nueva función para cambiar el modo de proyección
GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);
void printFPS();

// Variables globales (estados globales de la aplicación)
GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = false;
GLboolean lineMode = false;       // Variable para el modo "alambre"
GLboolean perspectiveMode = true; // Variable para el modo perspectiva
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;  // Ancho inicial de la ventana
GLint g_Height = 500; // Altura incial de la ventana

const GLfloat Kw = 100.0f; // Factor de escala

GLfloat camX = 0.0f;
GLfloat camY = 0.0f;
GLfloat camZ = 3.0f;


GLint main(GLint argc, char *argv[])
{
	glutInit(&argc, (char**)argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Ejercicios tema 1 - parte 1");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
 
	glutMainLoop();
 
	return EXIT_SUCCESS;
}
 
void drawBox()
{
	/* Un triangle strip dibuja un triangulo en un vertice usando los dos anteriores. Por lo que primero he hecho la cara superior,
	 * luego la derecha y por último la inferior, de forma que se vayan uniendo los triángulos correctamente.
	 * Me ayudé dibujando una tira de tríangulos en el paint para entender mejor el concepto.
	 * 
	 *  v0 v2  v4
	 *  | / | / |
 	 *  v1  v3  v5
	*/

	// Primera tira. Caras superior, derecha e inferior
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 0.0f, 1.0f);
		// Cara superior
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f,-0.5f);
		glVertex3f( 0.5f, 0.5f, 0.5f);
		glVertex3f( 0.5f, 0.5f,-0.5f);
		// Cara derecha
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.5f, -0.5f,  0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		// Cara inferior
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f,  0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
	glEnd();

	// Segunda tira. Caras trasera, izquierda y frontal
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 1.0f, 0.0f);
		// Cara trasera
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f,-0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f,-0.5f);
		glVertex3f(-0.5f,-0.5f,-0.5f);
		// Cara izquierda
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f,-0.5f, 0.5f);
		// Cara frontal
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f,-0.5f, 0.5f);

	glEnd();
 
	return;
}
 
GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	// El color del triangulo será el del último vértice especificado. Si se usa GL_SMOOTH, se interpola entre los colores de los vértices.
	glShadeModel(GL_FLAT); // Por defecto en GL_SMOOTH
 
	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
 
	/*gluLookAt(
	0.0f, 0.0f, 3.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);*/
 
	/*glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);*/

	gluLookAt(
	camX, camY, camZ,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);
 
	//drawBox();
	drawSolarSystem();
 
	glutSwapBuffers();
	return;
}
 
void resize(GLint w, GLint h)
{
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
*/
	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);

	
	/*
	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;*/
	projecTionMode();

	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
	return;
}
 
void idle()
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
		break;
	case 'v': case 'V':
		lineMode = !lineMode; // Alternar el modo "alambre"
		if (lineMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo alambre
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Modo relleno
		break;
	case 'x': case 'X':
		perspectiveMode = !perspectiveMode; // Alternar el modo perspectiva/paralela
		projecTionMode();
		break;
	}
	return;
}
 
void specialKeyboard(GLint key, GLint x, GLint y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;
 
		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(g_Width, g_Height);
			glutPositionWindow(50, 50);
		}
	}
	return;
}

void mouse(GLint button, GLint state, GLint x, GLint y)
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

void mouseMotion(GLint x, GLint y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;

		// Calcular nueva posición de cámara usando ángulos de Euler
		camX = 3.0f * sinf(yrot * 3.14159f / 180.0f) * cosf(xrot * 3.14159f / 180.0f);
		camY = 3.0f * sinf(xrot * 3.14159f / 180.0f);
		camZ = 3.0f * cosf(yrot * 3.14159f / 180.0f) * cosf(xrot * 3.14159f / 180.0f);

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

// Función para obtener el tiempo actual
GLfloat getTime()
{
	return glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void drawSolarSystem()
{
	GLfloat t = getTime();

	// Velocidades
	GLfloat w_tierra_eje = 1.0f * Kw;          // Rotación de la Tierra sobre su eje
	GLfloat w_tierra_sol = (1.0f/365.0f) * Kw; // Traslación de la Tierra alrededor del Sol
	GLfloat w_luna = (1.0f/28.0f) * Kw;        // Traslación de la Luna alrededor de la Tierra
	GLfloat w_sat_vertical = 2.0 * Kw;         // Rotación del satélite en vertical
	GLfloat w_sat_geo = 1.0f * Kw;             // Rotación del satélite en horizontal

	// Ángulos (ang = w * t)
	GLfloat ang_tierra_eje = w_tierra_eje * t;
	GLfloat ang_tierra_sol = w_tierra_sol * t;
	GLfloat ang_luna = w_luna * t;
	GLfloat ang_sat_vertical = w_sat_vertical * t;
	GLfloat ang_sat_geo = w_sat_geo * t;

	// Dibujar Sol
	glPushMatrix();
	    glScalef(0.5f, 0.5f, 0.5f); // Escalar el Sol
		drawBox();
	glPopMatrix();

	// Dibujar Tierra (y sus satélites)
    glPushMatrix();
        // Rotar alrededor del sol
        glRotatef(ang_tierra_sol, 0.0f, 1.0f, 0.0f);
        // Trasladar lejos del sol
        glTranslatef(2.0f, 0.0f, 0.0f);
        
        // Tierra
        glPushMatrix();
            // Rotar sobre su propio eje
            glRotatef(ang_tierra_eje, 0.0f, 1.0f, 0.0f);
            // Escalar (tierra más pequeña que el sol)
            glScalef(0.25f, 0.25f, 0.25f);
            drawBox();
        glPopMatrix();
        
        // Dibujar Luna
        glPushMatrix();
            // Rotar alrededor de la tierra
            glRotatef(ang_luna, 0.0f, 1.0f, 0.0f);
            // Trasladar lejos de la tierra
            glTranslatef(0.8f, 0.0f, 0.0f);
            // Escalar
            glScalef(0.125f, 0.125f, 0.125);
            drawBox();
        glPopMatrix();
        
        // Dibujar Sat. Vert.
        glPushMatrix();
            // Rotar en vertical a la tierra
            glRotatef(ang_sat_vertical, 1.0f, 0.0f, 0.0f);
            // Trasladar lejos de la tierra
            glTranslatef(0.0f, 0.4f, 0.0f);
            // Escalar
            glScalef(0.06f, 0.06f, 0.06f);
            drawBox();
        glPopMatrix();
        
        // Dibujar Sat. Geo.
        glPushMatrix();
            // Rotar a la misma velocidad que la tierra
            glRotatef(ang_sat_geo, 0.0f, 1.0f, 0.0f);
            // Trasladar
            glTranslatef(0.5f, 0.0f, 0.0f);
            // Escalar
            glScalef(0.05f, 0.05f, 0.05f);
            drawBox();
        glPopMatrix();
        
    glPopMatrix();

	return;
}

void projecTionMode()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect = (GLfloat)g_Width / (GLfloat)g_Height;

	if (perspectiveMode)
	{
		// Modo perspectiva
		gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	}
	else
	{
		GLfloat size = 1.125f;

		// Modo paralela
		if(aspect >= 1.0f)
		{
			// Ancho mayor que alto
			glOrtho(-size * aspect, size * aspect, -size, size, 0.1f, 100.0f);
			//glOrtho(-aspect, aspect, -aspect, aspect, 0.1f, 100.0f);
		}
		else
		{
			// Alto mayor que ancho
			glOrtho(-size, size, -size / aspect, size / aspect, 0.1f, 100.0f);
			//glOrtho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, 0.1f, 100.0f);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
void projecTionMode()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect = (GLfloat)g_Width / (GLfloat)g_Height;

	if (perspectiveMode)
	{
		// Modo perspectiva
		gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	}
	else
	{
		// Modo paralela: ajustar el tamaño basándose en la cámara perspectiva
		// Para mantener el tamaño del sol, usamos: tan(FOV/2) * distancia
		GLfloat fov = 45.0f * M_PI / 180.0f; // Convertir a radianes
		GLfloat distance = 3.0f; // Distancia de la cámara (gluLookAt)
		GLfloat size = tanf(fov / 2.0f) * distance;

		if(aspect >= 1.0f)
		{
			glOrtho(-size * aspect, size * aspect, -size, size, 0.1f, 100.0f);
		}
		else
		{
			glOrtho(-size, size, -size / aspect, size / aspect, 0.1f, 100.0f);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
*/