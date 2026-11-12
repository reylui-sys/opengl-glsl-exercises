// Ejercicios tema 2 - parte 1 (PTG - 2026)

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vboteapot.h"

void initCube(); 
GLint initTeapot(GLint grid, glm::mat4 transform);
void drawCube();
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
 
GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500; // Ancho inicial de la ventana
GLint g_Height = 500; // Altura incial de la ventana

GLuint vaoCube;
GLuint vaoTeapot; // Ejercicio 1.4

GLuint programID; // Objeto programa para el cubo
GLuint programTeapootID; // Objeto programa para la tetera

GLint locMVP_cube; // Localización de la variable uniform MVP para el cubo

// Ejercicio 1.4: Localizaciones de las variables uniform para la tetera
GLint locMVP_teapot;
GLint locMatrix_Rotation;
GLint locMorphFactor;
GLint locSphereRadius;
GLint locColorAngle;
GLint locDiscardOn;
GLint locDiscardThreshold;

GLint numVertTeapot; // número de vértices de la tetera

// Ejercicio 1.4
GLboolean showTeapot = false; // variable de control para mostrar/ocultar la tetera

// Ejercicio 1.5
GLboolean morphOn = false; // variable de control para activar/desactivar el morphing
GLfloat morphFactor = 0.0f; // factor de morphing (0.0f -> modelo original, 1.0f -> esfera)	
GLfloat morphDirection = 1.0f; // dirección del morphing (1.0f -> hacia la esfera, -1.0f -> hacia el modelo original)

// Ejercicio 1.6
GLboolean colorRotOn = false; // variable de control para activar/desactivar la rotación de colores
GLfloat colorAngle = 0.0f; // ángulo de rotación de colores

// Ejercicio 1.7
GLboolean discardOn = false; // variable de control para activar/desactivar el descarte de fragmentos
GLfloat discardTime = glm::pi<float>() / 2.0f; // variable de control para el tiempo en el que se activa el descarte de fragmentos (0.0f -> no se ha activado, > 0.0f -> tiempo transcurrido desde que se activó el descarte)


// cubo ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// Coordenadas del vertex array  =====================================
// Un cubo tiene 6 lados y cada lado tiene 2 triangles, por tanto, un cubo
// tiene 36 vértices (6 lados * 2 trian * 3 vertices = 36 vertices). Y cada
// vertice tiene 4 componentes (x,y,z,w) de reales, por tanto, el tamaño del vertex
// array es 144 floats (36 * 4 = 144).
GLfloat vertices1[] = { 1, 1, 1, 1,  -1, 1, 1, 1,  -1,-1, 1, 1,     // v0-v1-v2 (front)
                       -1,-1, 1, 1,   1,-1, 1, 1,   1, 1, 1, 1,     // v2-v3-v0

                        1, 1, 1, 1,   1,-1, 1, 1,   1,-1,-1, 1,      // v0-v3-v4 (right)
                        1,-1,-1, 1,   1, 1,-1, 1,   1, 1, 1, 1,      // v4-v5-v0

                        1, 1, 1, 1,   1, 1,-1, 1,  -1, 1,-1, 1,      // v0-v5-v6 (top)
                       -1, 1,-1, 1,  -1, 1, 1, 1,   1, 1, 1, 1,      // v6-v1-v0

                       -1, 1, 1, 1,  -1, 1,-1, 1,  -1,-1,-1, 1,      // v1-v6-v7 (left)
                       -1,-1,-1, 1,  -1,-1, 1, 1,  -1, 1, 1, 1,      // v7-v2-v1

                       -1,-1,-1, 1,   1,-1,-1, 1,   1,-1, 1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1, 1,  -1,-1, 1, 1,  -1,-1,-1, 1,      // v3-v2-v7

                        1,-1,-1, 1,  -1,-1,-1, 1,  -1, 1,-1, 1,      // v4-v7-v6 (back)
                       -1, 1,-1, 1,   1, 1,-1, 1,   1,-1,-1, 1 };    // v6-v5-v4
// color array
GLfloat colores1[]   = {1, 1, 1, 1,   1, 1, 0, 1,   1, 0, 0, 1,      // v0-v1-v2 (front)
                        1, 0, 0, 1,   1, 0, 1, 1,   1, 1, 1, 1,      // v2-v3-v0

                        1, 1, 1, 1,   1, 0, 1, 1,   0, 0, 1, 1,      // v0-v3-v4 (right)
                        0, 0, 1, 1,   0, 1, 1, 1,   1, 1, 1, 1,      // v4-v5-v0

                        1, 1, 1, 1,   0, 1, 1, 1,   0, 1, 0, 1,      // v0-v5-v6 (top)
                        0, 1, 0, 1,   1, 1, 0, 1,   1, 1, 1, 1,      // v6-v1-v0

                        1, 1, 0, 1,   0, 1, 0, 1,   0, 0, 0, 1,      // v1-v6-v7 (left)
                        0, 0, 0, 1,   1, 0, 0, 1,   1, 1, 0, 1,      // v7-v2-v1

                        0, 0, 0, 1,   0, 0, 1, 1,   1, 0, 1, 1,      // v7-v4-v3 (bottom)
                        1, 0, 1, 1,   1, 0, 0, 1,   0, 0, 0, 1,      // v3-v2-v7

                        0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 0, 1,      // v4-v7-v6 (back)
                        0, 1, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1 };    // v6-v5-v4



// BEGIN: Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////

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
	source = new std::string( std::istreambuf_iterator<char>(f),   
						std::istreambuf_iterator<char>() );
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
	glGetShaderiv( shadID, GL_COMPILE_STATUS, &compiled );
	if (compiled == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetShaderiv( shadID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetShaderInfoLog( shadID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader compiling failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void printLinkInfoLog(GLuint programID)
{
GLint linked;
	glGetProgramiv( programID, GL_LINK_STATUS, &linked );
	if(! linked)
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader linking failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
	return;
}

void validateProgram(GLuint programID)
{
GLint status;
    glValidateProgram( programID );
    glGetProgramiv( programID, GL_VALIDATE_STATUS, &status );

    if( status == GL_FALSE ) 
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

        if( infoLength > 0 ) 
		{
			GLchar *infoLog = new GLchar[infoLength];
			GLint chsWritten = 0;
            glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );
			std::cerr << "Program validating failed:" << infoLog << std::endl;
			system("pause");
            delete [] infoLog;

			exit(EXIT_FAILURE);
		}
    }
	return;
}

// END:   Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////


// BEGIN: Inicializa primitivas de dibujo //////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Init para Cube
///////////////////////////////////////////////////////////////////////////////
void initCube()
{
	GLuint vboHandle;

	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	// glEnableClientState(GL_VERTEX_ARRAY);	// Tarea por hacer (ejer. 1.2): sustituir por glEnableVertexAttribArray
	// glEnableClientState(GL_COLOR_ARRAY);	// Idem.

	// Ejercicio 1.2: Obtener location de los areibutos del shader del cubo
	GLint locPos = glGetAttribLocation(programID, "aPosition");
	GLint locCol = glGetAttribLocation(programID, "aColor");

	glGenBuffers(1, &vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(colores1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(colores1), colores1);

	// glVertexAttribPointer(locPos, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0);	// Tarea por hacer (ejer. 1.2): sustituir por glVertexAttribPointer
	// glColorAttribPointer(locCol, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + sizeof(GL_FLOAT) * 4 * 36); // Idem.

	// Ejercicio 1.2: Establecer los punteros a los atributos del shader del cubo
	glVertexAttribPointer( locPos, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0);
	glVertexAttribPointer( locCol, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + sizeof(GL_FLOAT) * 4 * 36);
	glEnableVertexAttribArray(locPos);
	glEnableVertexAttribArray(locCol);

	glBindVertexArray(0);
	return;
}

///////////////////////////////////////////////////////////////////////////////
// Init Teapot
// parametros: 
//		grid - n�mero de rejillas
//		transform - matriz de tranformaci�n del modelo
// return:
//		n�mero de vertices del modelo
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

	// Tarea por hacer (ejer. 1.4): Crear y activar VAO
	glGenVertexArrays(1, &vaoTeapot);
	glBindVertexArray(vaoTeapot);

	// Ejercicio 1.4: Obtener locations
	GLint locPos = glGetAttribLocation(programTeapootID, "aPosition");
	GLint locNorm = glGetAttribLocation(programTeapootID, "aNormal");
	GLint locTex = glGetAttribLocation(programTeapootID, "aTexCoord");

	GLuint handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), v, GL_STATIC_DRAW); // Datos de la posici�n de los v�rtices
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locPos);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(GLfloat), n, GL_STATIC_DRAW); // Datos de las normales de los v�rtices
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	glVertexAttribPointer(locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locNorm);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(GLfloat), tc, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	// Tarea por hacer (ejer. 1.4): Establecer el puntero al atributo del vertex shader y activar el array
	glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locTex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW); // Array de �ndices

	delete[] v;
	delete[] n;
	delete[] el;
	delete[] tc;

	// Tarea por hacer (ejer. 1.4): Desactivar VAO
	glBindVertexArray(0);

	return 6 * faces;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////


// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dibuja cubo utilizando VAO (no indexado)
///////////////////////////////////////////////////////////////////////////////
void drawCube()
{
	glBindVertexArray(vaoCube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); 
	return;
}

///////////////////////////////////////////////////////////////////////////////
// Dibuja tetera utilizando VAO (indexado)
///////////////////////////////////////////////////////////////////////////////
void drawTeapot() {
	// Tarea por hacer (ejer. 1.4): Dibujar la tetera
	glBindVertexArray(vaoTeapot);
	glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, 0);
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

	glShadeModel(GL_SMOOTH);

	// Tarea por hacer (ejer. 1.1): Descomentar el siguiente c�digo

	//  Creamos el objeto shader compilado para el shader de vertice.
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "./shaders/tema2_parte1.vert");
	std::cout << "Compilando shader de vertice ..." << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	
	// Tarea por hacer (ejer. 1.1): Creamos el objeto shader compilado para el shader de fragmento.
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "./shaders/tema2_parte1.frag");
	std::cout << "Compilando shader de fragmento ..." << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);

	// Tarea por hacer (ejer. 1.1): Creamos el objeto programa.
	programID = glCreateProgram();
	
	// Tarea por hacer (ejer. 1.1): Adjuntamos los shaders compilados al objeto programa
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	// Tarea por hacer (ejer. 1.1): Linkamos el objeto programa
	glLinkProgram(programID);
	
	// Tarea por hacer (ejer. 1.1): Descomentar el siguiente c�digo (verifica que no hay errores en el linkado)
	std::cout << "Linkando objeto programa ..." << std::endl;
	printLinkInfoLog(programID);
	validateProgram(programID);

	// Tarea por hacer (ejer. 1.3): Localizar las variables uniform definidas en los shaders.
	locMVP_cube = glGetUniformLocation(programID, "mvp");
	initCube();

	// Ejercicio 1.4: Repetir el proceso de creación de shaders, programa y localización de variables uniform para la tetera
	GLuint vertexShaderTeapotID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderTeapotID, "./shaders/teapot.vert");
	std::cout << "Compilando shader de vertice para la tetera..." << std::endl;
	glCompileShader(vertexShaderTeapotID);
	printCompileInfoLog(vertexShaderTeapotID);

	GLuint fragmentShaderTeapotID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderTeapotID, "./shaders/teapot.frag");
	std::cout << "Compilando shader de fragmento para la tetera..." << std::endl;
	glCompileShader(fragmentShaderTeapotID);
	printCompileInfoLog(fragmentShaderTeapotID);

	programTeapootID = glCreateProgram();
	glAttachShader(programTeapootID, vertexShaderTeapotID);
	glAttachShader(programTeapootID, fragmentShaderTeapotID);
	glLinkProgram(programTeapootID);
	std::cout << "Linkando objeto programa para la tetera..." << std::endl;
	printLinkInfoLog(programTeapootID);
	validateProgram(programTeapootID);

	// Ejercicio 1.4: Localizar variables uniform para la tetera
	locMVP_teapot = glGetUniformLocation(programTeapootID, "mvp");
	locMatrix_Rotation = glGetUniformLocation(programTeapootID, "matrixRotation");
	locMorphFactor = glGetUniformLocation(programTeapootID, "morphFactor");
	locSphereRadius = glGetUniformLocation(programTeapootID, "sphereRadius");
	locColorAngle = glGetUniformLocation(programTeapootID, "colorAngle");
	locDiscardOn = glGetUniformLocation(programTeapootID, "discardOn");
	locDiscardThreshold = glGetUniformLocation(programTeapootID, "discardThreshold");

	// Tarea por hacer (ejer. 1.4): Descomentar el siguiente c�digo
	numVertTeapot = initTeapot(10, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f)));

	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);

	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();

	// gluLookAt(
	// 0.0f, 0.0f, 5.0f,
	// 0.0f, 0.0f, 0.0f,
	// 0.0f, 1.0f, 0.0f);

	// glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	// glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	// Ya no se usa la pipelina fija, por tanto, hay que calcular la matriz de transformaci�n completa (MVP) y pasarla a los shaders a trav�s de variables uniform. Para ello, se pueden usar las funciones de GLM para calcular las matrices de proyecci�n, vista y modelo, y luego multiplicarlas para obtener la matriz MVP.

	// Tarea por hacer (ejer. 1.3): Sustituir el anterior c�digo por el c�digo siguiente.
	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	Model = glm::rotate(Model, xrot, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, yrot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mvp = Projection * View * Model;

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), colorAngle, glm::vec3(1.0f, 0.0f, 0.0f));

	if(showTeapot)
	{
		// Tarea por hacer (ejer. 1.4): Activar el objeto programa
		glUseProgram(programTeapootID);

		// Tarea por hacer (ejer. 1.4): Pasarle a los shaders las variables uniform.
		glUniformMatrix4fv(locMVP_teapot, 1, GL_FALSE, glm::value_ptr(mvp));
		
		glUniformMatrix4fv(locMatrix_Rotation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
		// Ejercicio 1.5: Pasar el factor de morphing a los shaders
		glUniform1f(locMorphFactor, morphFactor);
		glUniform1f(locSphereRadius, 3.0f); // Radio de la esfera para el morphing (se puede ajustar para obtener mejores resultados)

		// Ejercicio 1.6: Pasar el ángulo de rotación de colores a los shaders
		glUniform1f(locColorAngle, colorAngle);

		// Ejercicio 1.7: Pasar las variables de control para el descarte de fragmentos a los shaders
		glUniform1i(locDiscardOn, discardOn ? 1 : 0);
		float threshold = (sin(discardTime) + 1.0f) / 2.0f; // Umbral que varía entre 0.0f y 1.0f en función del tiempo
		glUniform1f(locDiscardThreshold, threshold);
		
		drawTeapot();
	}
	else
	{
		// Tarea por hacer (ejer. 1.1): Activar el objeto programa
		glUseProgram(programID);
		// Pasar a los shaders las variables uniform.
		glUniformMatrix4fv(locMVP_cube, 1, GL_FALSE, glm::value_ptr(mvp));
		// Tarea por hacer (ejer. 1.3): Pasarle a los shaders las variables uniform.	
		drawCube();
	}

	// Tarea por hacer (ejer. 1.1): Desactivar el objeto programa	
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
		xrot += 0.0010f;
		yrot += 0.0012f;
	}

	// Ejercicio 1.5: Actualizar el factor de morphing en función del tiempo
	if(morphOn)
	{
		morphFactor += morphDirection * 0.001f; // Ajustar la velocidad del morphing
		if(morphFactor >= 1.0f)
		{
			morphFactor = 1.0f;
			morphDirection = -1.0f; // Cambiar dirección para volver al modelo original
		}
		else if(morphFactor <= 0.0f)
		{
			morphFactor = 0.0f;
			morphDirection = 1.0f; // Cambiar dirección para volver a la esfera
		}
	}

	// Ejercicio 1.6: Actualizar el ángulo de rotación de colores en función del tiempo
	if(colorRotOn)
	{
		colorAngle += 0.001f; // Ajustar la velocidad de rotación de colores
		// if(colorAngle >= 360.0f)
		// 	colorAngle -= 360.0f;
	}

	// Ejercicio 1.7: Actualizar el tiempo para el descarte de fragmentos
	if(discardOn)
		discardTime += 0.0025f; // Ajustar la velocidad de cambio del umbral de descarte

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
	case 't': case 'T':
		showTeapot = !showTeapot;
		std::cout << "Mostrar tetera: " << (showTeapot ? "Si" : "No") << std::endl;
		break;
	case 'p': case 'P':
		morphOn = !morphOn;
		std::cout << "Morphing: " << (morphOn ? "Activado" : "Desactivado") << std::endl;
		break;
	case 'c': case 'C':
		colorRotOn = !colorRotOn;
		if(!colorRotOn) colorAngle = 0.0f; // Reiniciar el ángulo de rotación de colores al desactivar la rotación
		std::cout << "Rotacion de colores: " << (colorRotOn ? "Activada" : "Desactivada") << std::endl;
		break;
	case 'f': case 'F':
		discardOn = !discardOn;
		if(!discardOn) discardTime = glm::pi<float>() / 2.0f; // Reiniciar el tiempo de descarte al desactivar el descarte
		std::cout << "Descarte de fragmentos: " << (discardOn ? "Activado" : "Desactivado") << std::endl;
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

