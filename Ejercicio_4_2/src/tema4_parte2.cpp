// Ejercicios del tema 4 - parte 2 (PTG - 2026)

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vboteapot.h"
#include "teapotdata.h"
#include "lodepng.h"

GLint initTeapot(GLint grid, glm::mat4 transform);
void drawTeapot();

void loadSource(GLuint &shaderID, std::string name);
void printCompileInfoLog(GLuint shadID);
void printLinkInfoLog(GLuint programID);
void validateProgram(GLuint programID);

GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);


GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = false;

// flags para ejercicios 2.2 y 2.3
GLboolean explosionOn = false;  // activa/desactiva la explosión
GLboolean impostorOn = false;  // activa/desactiva los impostores

GLboolean reverseExplosionOn = false; 

GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLuint teapotVAOHandle;
GLuint programID1, programID2;
GLuint locUniformMVPM, locUniformMVM, locUniformNM, locUniformPM;
GLuint locUniformSpriteTex;
GLuint locUniformExplosionFactor;

GLfloat factor_explosion = 0;

GLint numVertTeapot;

// BEGIN: Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////

void loadSource(GLuint &shaderID, std::string name)
{
	std::ifstream f(name.c_str());
	if (!f.is_open())
	{
		std::cerr << "File not found " << name.c_str() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	// now read in the data
	std::string *source;
	source = new std::string(std::istreambuf_iterator<GLchar>(f),
		std::istreambuf_iterator<GLchar>());
	f.close();

	// add a null to the string
	*source += "\0";
	const GLchar * data = source->c_str();
	glShaderSource(shaderID, 1, &data, NULL);
	delete source;
	return;
}

void printCompileInfoLog(GLuint shadID)
{
	GLint compiled;
	glGetShaderiv(shadID, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetShaderiv(shadID, GL_INFO_LOG_LENGTH, &infoLength);

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetShaderInfoLog(shadID, infoLength, &chsWritten, infoLog);

		std::cerr << "Shader compiling failed:" << infoLog << std::endl;
		system("pause");
		delete[] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void printLinkInfoLog(GLuint programID)
{
	GLint linked;
	glGetProgramiv(programID, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetProgramInfoLog(programID, infoLength, &chsWritten, infoLog);

		std::cerr << "Shader linking failed:" << infoLog << std::endl;
		system("pause");
		delete[] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void validateProgram(GLuint programID)
{
	GLint status;
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);

		if (infoLength > 0)
		{
			GLchar *infoLog = new GLchar[infoLength];
			GLint chsWritten = 0;
			glGetProgramInfoLog(programID, infoLength, &chsWritten, infoLog);
			std::cerr << "Program validating failed:" << infoLog << std::endl;
			system("pause");
			delete[] infoLog;

			exit(EXIT_FAILURE);
		}
	}
	return;
}

// END:   Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Init Teapot
// parametros: 
//		grid - n�mero de rejillas
//		transform - matriz de tranformaci�n del modelo
// return:
//		n�mero de vertices
///////////////////////////////////////////////////////////////////////////////
GLint initTeapot(GLint grid, glm::mat4 transform)
{
	GLint verts = 32 * (grid + 1) * (grid + 1);
	GLint faces = grid * grid * 32;
	GLfloat * v = new GLfloat[verts * 3];
	GLfloat * n = new GLfloat[verts * 3];
	GLfloat * tc = new GLfloat[verts * 2];
	GLuint * el = new GLuint[faces * 6];

	generatePatches(v, n, tc, el, grid);
	moveLid(grid, v, transform);

	glGenVertexArrays(1, &teapotVAOHandle);
	glBindVertexArray(teapotVAOHandle);

	GLuint handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), v, GL_STATIC_DRAW); // Datos de la posici�n de los v�rtices
	// GLuint loc1 = glGetAttribLocation(programID, "aPosition");
	// No es necesario localizarla porque se le ha asignado como ID = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), n, GL_STATIC_DRAW); // Datos de las normales de los v�rtices
	// GLuint loc2 = glGetAttribLocation(programID, "aNormal");
		// No es necesario localizarla porque se le ha asignado como ID = 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(GLuint), el, GL_STATIC_DRAW); // Array de �ndices

	delete[] v;
	delete[] n;
	delete[] el;
	delete[] tc;

	glBindVertexArray(0);

	return 6 * faces;
}
// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////
void drawTeapot() {
	glBindVertexArray(teapotVAOHandle);
	glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
	glBindVertexArray(0);
	return;
}
// END: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	  system("pause");
	  exit(-1);
	}
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

GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glShadeModel(GL_SMOOTH);

	// PROGRAMA 1 – puntos -> .vert + .geom (salida ùntos) + .frag
    programID1 = glCreateProgram();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "shaders\\tema4_parte2.vert");
	std::cout << "Compiling Vertex Shader" << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(programID1, vertexShaderID);

    GLuint geomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    loadSource(geomShaderID, "shaders\\tema4_parte2.geom");
    std::cout << "Compiling Geometry Shader (points)" << std::endl;
    glCompileShader(geomShaderID);
    printCompileInfoLog(geomShaderID);
    glAttachShader(programID1, geomShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "shaders\\tema4_parte2.frag");
	std::cout << "Compiling Fragment Shader" << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(programID1, fragmentShaderID);


	glLinkProgram(programID1);
	printLinkInfoLog(programID1);
	validateProgram(programID1);

    //  PROGRAMA 2 – impostores / esferas -> mismo .vert + .geom distinto (salida triangle_strip) + .frag distinto
    //  Necesita un programa SEPARADO porque el tipo de primitiva de salida del GS (points vs triangle_strip) no puede cambiar en tiempo de ejecución
    programID2 = glCreateProgram();

    // Reutilizamos el mismo vertex shader
    GLuint vertexShaderID2 = glCreateShader(GL_VERTEX_SHADER);
    loadSource(vertexShaderID2, "shaders\\tema4_parte2.vert");
    glCompileShader(vertexShaderID2);
    printCompileInfoLog(vertexShaderID2);
    glAttachShader(programID2, vertexShaderID2);

    // GS diferente: emite triangle_strip (quad billboard)
    GLuint geomShaderID2 = glCreateShader(GL_GEOMETRY_SHADER);
    loadSource(geomShaderID2, "shaders\\tema4_parte2_impostors.geom");
    std::cout << "Compiling Geometry Shader (impostors)" << std::endl;
    glCompileShader(geomShaderID2);
    printCompileInfoLog(geomShaderID2);
    glAttachShader(programID2, geomShaderID2);

    // FS diferente: descarta fragmentos fuera de la esfera y aplica textura
    GLuint fragmentShaderID2 = glCreateShader(GL_FRAGMENT_SHADER);
    loadSource(fragmentShaderID2, "shaders\\tema4_parte2_impostors.frag");
    std::cout << "Compiling Fragment Shader (impostors)" << std::endl;
    glCompileShader(fragmentShaderID2);
    printCompileInfoLog(fragmentShaderID2);
    glAttachShader(programID2, fragmentShaderID2);

    glLinkProgram(programID2);
    printLinkInfoLog(programID2);
    validateProgram(programID2);

	// Descomentar y completar el código para la carga de la imagen con el impostor.
	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;
	GLuint error;
	const GLchar img_filename[] = "texturas\\white_sphere.png";

	GLuint textId;
	glGenTextures(1, &textId);

	error = lodepng::decode(img_data, img_width, img_height, img_filename);
	if (!error)
	{
		glBindTexture(GL_TEXTURE_2D, textId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
						img_width, img_height, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, img_data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Activa la unidad 0 y enlaza el sampler uSpriteTex (location=5) a ella
		glUseProgram(programID2);
		glUniform1i(5 /*locUniformSpriteTex*/, 0);
		glUseProgram(0);

		std::cout << "Textura cargada: " << img_filename
					<< " (" << img_width << "x" << img_height << ")" << std::endl;
	}
	else
		std::cout << "Error al cargar la textura " << img_filename << std::endl;
	img_data.clear();

	numVertTeapot = initTeapot(5, glm::mat4(1.0f));

	// No es necesario localizar el ID porque se le ha asignado en los shaders un ID fijo  (ver c�digo de los shaders)
	/*
	locUniformMVPM = glGetUniformLocation(programID, "uModelViewProjMatrix");
	locUniformMVM = glGetUniformLocation(programID, "uModelViewMatrix");
	locUniformNM = glGetUniformLocation(programID, "uNormalMatrix");
	locUniformPM = glGetUniformLocation(programID, "uProjectionMatrix");
	locUniformExplosionFactor = glGetUniformLocation(programID, "uExplosionFactor");
	locUniformSpriteTex = glGetUniformLocation(programID, "uSpriteTex");
	*/
	// En lugar de ello le ponemos directamente el ID que le hemos asignado en el c�digo de los shaders
	locUniformMVPM = 0;
	locUniformMVM = 1;
	locUniformNM = 2;
	locUniformPM = 3;
	locUniformExplosionFactor = 4;
	locUniformSpriteTex = 5;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT_FACE);

	glPointSize(3.0f);

	return true;
}
 
void display()
{
	// Constante con la cantidad que tiene que incrementarse factor_explosion en cada frame
	const GLfloat inc_explosion = 0.002;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	
	glm::vec3 cameraPos = glm::vec3( 9.0f * cos( yrot / 100 ), 2.0f * sin(xrot / 100) + 2.0f, 9.0f * sin( yrot / 100 ) * cos(xrot /100) );
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ModelTeapot = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), vec3(0.25, 0.25, 0.25)), -90.0f, vec3(1.0, 0.0, 0.0)), vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 mvp; // Model-view-projection matrix
	glm::mat4 mv;  // Model-view matrix
	glm::mat3 nm;  // Normal matrix

	// glUseProgram(programID1);

    // programID1, puntos
    // programID2, esferas billboard
    GLuint activeProgram = impostorOn ? programID2 : programID1;
    glUseProgram(activeProgram);

	// Paso al shader de los valores para las variables uniform
	mv = View;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	

	// Dibuja Teapot
	mvp = Projection * View * ModelTeapot;
	mv = View * ModelTeapot;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMVM, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);
	glUniformMatrix4fv(locUniformPM, 1, GL_FALSE, &Projection[0][0]);

	if(reverseExplosionOn)
	{
		if (explosionOn)
			factor_explosion += inc_explosion;
		else if (factor_explosion > 0)
		{
			factor_explosion -= inc_explosion;
			if (factor_explosion < 0)
				factor_explosion = 0.0f;
		}	
	}
	else
	{
		if (explosionOn)
			factor_explosion += inc_explosion;
		else 
			factor_explosion = 0.0f;
	}

    glUniform1f(locUniformExplosionFactor, factor_explosion);

	drawTeapot();

	glUseProgram(0);

	glutSwapBuffers();
	return;
}
 
void resize(GLint w, GLint h)
{
	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
	return;
}

void idle()
{
	if (!mouseDown && animation)
	{
		xrot += 0.3f;
		yrot += 0.4f;
	}
	glutPostRedisplay();
	return;
}

void keyboard(GLubyte key, GLint x, GLint y)
{
	static bool wireframe = false;
	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(EXIT_SUCCESS); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
    // Al desactivar, factor_explosion vuelve gradualmente a 0 (la tetera regresa a su posición inicial).
    case 'e': case 'E':
        explosionOn = !explosionOn;
        break;
	case 'r': case 'R':
		reverseExplosionOn = !reverseExplosionOn;
		break;
    // Alterna entre modo puntos (programID1) y modo impostores (programID2).
    case 'i': case 'I':
        impostorOn = !impostorOn;
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

		glutPostRedisplay();
	}
	return;
}
