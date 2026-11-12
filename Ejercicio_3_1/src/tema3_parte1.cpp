// Ejercicios tema 3 - parte 1 (PTG - 2026)

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vboteapot.h"
#include "teapotdata.h"
#include "lodepng.h"

void initCube();
GLint initSphere(GLfloat radius, GLuint rings, GLuint sectors);
GLint initTeapot(GLint grid, glm::mat4 transform);
void drawCube();
void drawSphere();
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

GLint g_Width = 500;                          // Ancho inicial de la ventana
GLint g_Height = 500;                         // Altura incial de la ventana

GLuint cubeVAOHandle;
GLuint sphereVAOHandle;
GLuint teapotVAOHandle;
GLuint programID;

// Locations de las uniforms
GLint locUniformStone, locUniformMoss, locUniformMap;
GLint locUniformMVPM, locUniformMM, locUniformNM;
GLint locUniformDrawSky, locUniformCamera;

GLint locUniformRenderMode;
GLint locUniformChromatic, locUniformFresnel;
GLint locUniformEtaR, locUniformEtaG, locUniformEtaB;

GLint locUniformFresnelF;

int numVertTeapot, numVertSphere;
GLuint textIds[3]; // 0 = stone, 1 = moss, 2 = cubemap
int renderMode = 0; // 0 = texturas, 1 = reflejo, 2 = refracción
bool chromaticOn = false; // Dispersión cromática
bool fresnelOn = false; // Bandera para el Fresnel

// Índices de refracción del vidrio
float refractionFactor = 2.0f;
float etaAir = 1.0f; // Indice del aire para calcular la relación
float etaGlassR = 1.506f;  // Rojo
float etaGlassG = 1.520f;  // Verde
float etaGlassB = 1.535f;  // Azul

// float etaGlassR = 1.3;  // Rojo
// float etaGlassG = 1.521f;  // Verde (también usado como índice medio)
// float etaGlassB = 1.8;  // Azul

// cubo ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,
                       -1,-1, 1,   1,-1, 1,   1, 1, 1,
                        1, 1, 1,   1,-1, 1,   1,-1,-1,
                        1,-1,-1,   1, 1,-1,   1, 1, 1,
                        1, 1, 1,   1, 1,-1,  -1, 1,-1,
                       -1, 1,-1,  -1, 1, 1,   1, 1, 1,
                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,
                       -1,-1,-1,  -1,-1, 1,  -1, 1, 1,
                       -1,-1,-1,   1,-1,-1,   1,-1, 1,
                        1,-1, 1,  -1,-1, 1,  -1,-1,-1,
                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,
                       -1, 1,-1,   1, 1,-1,   1,-1,-1 };

GLfloat normales1[] = { 0, 0, 1,   0, 0, 1,   0, 0, 1,
                        0, 0, 1,   0, 0, 1,   0, 0, 1,
                        1, 0, 0,   1, 0, 0,   1, 0, 0,
                        1, 0, 0,   1, 0, 0,   1, 0, 0,
                        0, 1, 0,   0, 1, 0,   0, 1, 0,
                        0, 1, 0,   0, 1, 0,   0, 1, 0,
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
                        0,-1, 0,   0,-1, 0,   0,-1, 0,
                        0,-1, 0,   0,-1, 0,   0,-1, 0,
                        0, 0,-1,   0, 0,-1,   0, 0,-1,
                        0, 0,-1,   0, 0,-1,   0, 0,-1 };

GLfloat texCoord1[] = { 1, 1,  0, 1,  0, 0,
                        0, 0,  1, 0,  1, 1,
                        1, 1,  1, 0,  0, 0,
                        0, 0,  0, 1,  1, 1,
                        1, 0,  1, 1,  0, 1,
                        0, 1,  0, 0,  1, 0,
                        0, 1,  1, 1,  1, 0,
                        1, 0,  0, 0,  0, 1,
                        0, 0,  1, 0,  1, 1,
                        1, 1,  0, 1,  0, 0,
                        0, 0,  1, 0,  1, 1,
                        1, 1,  0, 1,  0, 0 };


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
	std::string *source;
	source = new std::string( std::istreambuf_iterator<GLchar>(f),   
						std::istreambuf_iterator<GLchar>() );
	f.close();
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

// END: Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Init para drawCube_VAO
///////////////////////////////////////////////////////////////////////////////
void initCube() 
{
	GLuint vboHandle;

	glGenVertexArrays(1, &cubeVAOHandle);
	glBindVertexArray(cubeVAOHandle);

	glGenBuffers(1, &vboHandle); 
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);    
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(normales1) + sizeof(texCoord1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(normales1), normales1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(normales1), sizeof(texCoord1), texCoord1);

	// Tarea a realizar (ejercicio 1.1): Localizar las variables in del vertex shader para la posici�n,
	// la normal y las coordenadas de textura, habilitar estos atributos y establecer los punteros
	GLint locPos  = glGetAttribLocation(programID, "aPosition");
	GLint locNorm = glGetAttribLocation(programID, "aNormal");
	GLint locTex  = glGetAttribLocation(programID, "aTexCoord");

	// Ejercicio 1.1: Establecer punteros y habilitar atributos
	glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0);
	glEnableVertexAttribArray(locPos);

	glVertexAttribPointer(locNorm, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + sizeof(vertices1));
	glEnableVertexAttribArray(locNorm);

	glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + sizeof(vertices1) + sizeof(normales1));
	glEnableVertexAttribArray(locTex);

	glBindVertexArray(0);
	return;
}

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
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): Localizar la variable in del vertex shader para la posici�n,
	// habilitar los array de v�rtices para el atributo y establecer el puntero
	GLint locPos  = glGetAttribLocation(programID, "aPosition");
	glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locPos);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): idem para la normal.
	GLint locNorm = glGetAttribLocation(programID, "aNormal");
	glVertexAttribPointer(locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locNorm);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(float), tc, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): idem para la las coordenadas de textura.
	GLint locTex  = glGetAttribLocation(programID, "aTexCoord");
	glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locTex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW);

	delete [] v;
	delete [] n;
	delete [] el;
	delete [] tc;

	glBindVertexArray(0);
	return 6 * faces;
}

///////////////////////////////////////////////////////////////////////////////
// Init Sphere
// parametros: 
//		radius - radio de la esfera
//      rings - n�mero de anillos paralelos
//		sectors - numero de divisiones de los anillos
// return:
//		n�mero de vertices
///////////////////////////////////////////////////////////////////////////////
GLint initSphere(GLfloat radius, GLuint rings, GLuint sectors)
{
	const GLfloat R = 1.0f/(GLfloat)(rings-1);
	const GLfloat S = 1.0f/(GLfloat)(sectors-1);
	const GLdouble PI = 3.14159265358979323846;

	GLfloat *sphere_vertices = new GLfloat[rings * sectors * 3];
	GLfloat *sphere_normals = new GLfloat[rings * sectors * 3];
	GLfloat *sphere_texcoords = new GLfloat[rings * sectors * 2];
	GLfloat *vv = sphere_vertices;
	GLfloat *nn = sphere_normals;
	GLfloat *tt = sphere_texcoords;
	for(GLuint r = 0; r < rings; r++) for(GLuint s = 0; s < sectors; s++) {
		GLfloat const y = GLfloat( sin( -PI/2 + PI * r * R ) );
		GLfloat const x = GLfloat( cos(2*PI * s * S) * sin( PI * r * R ) );
		GLfloat const z = GLfloat( sin(2*PI * s * S) * sin( PI * r * R ) );
		*tt++ = s*S;
		*tt++ = r*R;
		*vv++ = x * radius;
		*vv++ = y * radius;
		*vv++ = z * radius;
		*nn++ = x;
		*nn++ = y;
		*nn++ = z;
	}

	GLushort *sphere_indices = new GLushort[rings * sectors * 4];
	GLushort *ii = sphere_indices;
	for(GLuint r = 0; r < rings; r++) for(GLuint s = 0; s < sectors; s++) {
		*ii++ = r * sectors + s;
		*ii++ = r * sectors + (s+1);
		*ii++ = (r+1) * sectors + (s+1);
		*ii++ = (r+1) * sectors + s;
	}

	glGenVertexArrays(1, &sphereVAOHandle);
	glBindVertexArray(sphereVAOHandle);

	GLuint handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_vertices, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): Localizar la variable in del vertex shader para la posici�n,
	// habilitar los array de v�rtices para el atributo y establecer el puntero
	GLint locPos  = glGetAttribLocation(programID, "aPosition");
	glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locPos);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_normals, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): idem para la normal.
	GLint locNorm = glGetAttribLocation(programID, "aNormal");
	glVertexAttribPointer(locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locNorm);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 2) * sizeof(float), sphere_texcoords, GL_STATIC_DRAW);
	// Tarea a realizar (ejercicio 1.1): idem para la las coordenadas de textura.
	GLint locTex  = glGetAttribLocation(programID, "aTexCoord");
	glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(locTex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rings * sectors * 4 * sizeof(GLushort), sphere_indices, GL_STATIC_DRAW);

	delete [] sphere_vertices;
	delete [] sphere_normals;
	delete [] sphere_texcoords;
	delete [] sphere_indices;

	glBindVertexArray(0);
	return rings * sectors * 4;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

void drawCube()
{
	// Tarea a realizar (ejercicio 1.1): Activar el VAO y dibujar los triángulos.
	glBindVertexArray(cubeVAOHandle);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); 
	return;
}

void drawTeapot()
{
	glBindVertexArray(teapotVAOHandle);
	glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
	glBindVertexArray(0);
	return;
}

void drawSphere()
{
	glBindVertexArray(sphereVAOHandle);
	glDrawElements(GL_QUADS, numVertSphere, GL_UNSIGNED_SHORT, ((GLubyte *)NULL + (0)));
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
	glutCreateWindow("Tema 3 - Texturas, Reflejos y Refraccion");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
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

	programID = glCreateProgram();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "../shaders/tema3_parte1.vert");
	loadSource(vertexShaderID, "C:/PTG/Ejercicio_3_1/proyecto/shaders/tema3_parte1.vert");
	std::cout << "Compilando vertex shader ..." << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(programID, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "/shaders/tema3_parte1.frag");
	// loadSource(fragmentShaderID, "C:/PTG/Ejercicio_3_1/proyecto/shaders/tema3_parte1.frag");
	std::cout << "Compilando fragment shader ..." << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);
	std::cout << "Linkando programa ..." << std::endl;
	printLinkInfoLog(programID);

	initCube();

	numVertTeapot = initTeapot(10, glm::mat4(1.0f));
	numVertSphere = initSphere(1.0f, 30, 60);

	// Inicializa Texturas
	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;
	GLuint error;

	const GLchar* img_filename_tex1 = "../texturas/stone.png";
	const GLchar* img_filename_tex2 = "../texturas/moss.png";

	const GLchar* img_filename_px = "../texturas/t2posx.png";
	const GLchar* img_filename_nx = "../texturas/t2negx.png";
	const GLchar* img_filename_py = "../texturas/t2posy.png";
	const GLchar* img_filename_ny = "../texturas/t2negy.png";
	const GLchar* img_filename_pz = "../texturas/t2posz.png";
	const GLchar* img_filename_nz = "../texturas/t2negz.png";

	// const GLchar* img_filename_tex1 = "C:/PTG/Ejercicio_3_1/proyecto/texturas/stone.png";
	// const GLchar* img_filename_tex2 = "C:/PTG/Ejercicio_3_1/proyecto/texturas/moss.png";

	// const GLchar* img_filename_px = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2posx.png";
	// const GLchar* img_filename_nx = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2negx.png";
	// const GLchar* img_filename_py = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2posy.png";
	// const GLchar* img_filename_ny = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2negy.png";
	// const GLchar* img_filename_pz = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2posz.png";
	// const GLchar* img_filename_nz = "C:/PTG/Ejercicio_3_1/proyecto/texturas/t2negz.png";

	glGenTextures(3, textIds);

	// Textura piedra
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textIds[0]);

	error = lodepng::decode(img_data, img_width, img_height, img_filename_tex1);
	if (!error)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
	else std::cout << "Error al cargar la textura " << img_filename_tex1 << std::endl;
	img_data.clear();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Textura musgo
	// Tarea a realizar (ejercicio 1.1): Fijar la unidad de textura, activar el objeto textura, cargar en �l los datos de la imagen y definir los par�metros de la textura
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textIds[1]);

	error = lodepng::decode(img_data, img_width, img_height, img_filename_tex2);
	if (!error)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
	else std::cout << "Error al cargar la textura " << img_filename_tex2 << std::endl;
	img_data.clear();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Mapa de entorno
	// Tarea a realizar (ejercicio 1.2): hacer lo mismo para el mapa de entorno.
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textIds[2]);

	const GLchar* faces_filenames[6] = {
		img_filename_px, img_filename_nx,
		img_filename_py, img_filename_ny,
		img_filename_pz, img_filename_nz
	};
	GLenum faces_targets[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	for (int i = 0; i < 6; i++)
	{
		error = lodepng::decode(img_data, img_width, img_height, faces_filenames[i]);
		if (!error)
			glTexImage2D(faces_targets[i], 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
		else std::cout << "Error al cargar la textura " << faces_filenames[i] << std::endl;
		img_data.clear();
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Tarea a realizar: localizar las variables uniform definidas en los shaders.
	locUniformMVPM = glGetUniformLocation(programID, "uModelViewProjMatrix");
	locUniformMM = glGetUniformLocation(programID, "uModelMatrix");
	locUniformNM = glGetUniformLocation(programID, "uNormalMatrix");
	locUniformCamera = glGetUniformLocation(programID, "uCameraPos");
	locUniformDrawSky = glGetUniformLocation(programID, "uDrawSky");
	locUniformStone = glGetUniformLocation(programID, "uStoneTex");
	locUniformMoss = glGetUniformLocation(programID, "uMossTex");
	locUniformMap = glGetUniformLocation(programID, "uTexMap");
	locUniformRenderMode = glGetUniformLocation(programID, "uRenderMode");
	locUniformChromatic = glGetUniformLocation(programID, "uChromatic");
	locUniformFresnel = glGetUniformLocation(programID, "uFresnel");
	locUniformEtaR = glGetUniformLocation(programID, "uEtaR");
	locUniformEtaG = glGetUniformLocation(programID, "uEtaG");
	locUniformEtaB = glGetUniformLocation(programID, "uEtaB");

	locUniformFresnelF = glGetUniformLocation(programID, "uFresnelF");

	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);

	glm::vec3 cameraPos = vec3(7.0f * cos(yrot / 100), 2.0f * sin(xrot / 100), 7.0f * sin(yrot / 100) * cos(xrot / 100));
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ModelSky = glm::scale(glm::mat4(1.0), glm::vec3(25.0f, 25.0f, 25.0f));
	glm::mat4 ModelCube = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(0.4f, 0.4f, 0.4f)), vec3(-3.0f, 0.0f, 3.0f));
	glm::mat4 ModelSphere = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	glm::mat4 ModelTeapot = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), vec3(0.25, 0.25, 0.25)), -90.0f, vec3(1.0, 0.0, 0.0)), vec3(4.0f, 0.0f, 2.0f));

	glm::mat4 mvp;
	glm::mat3 nm;

	glUseProgram(programID);

	// Tareas a realizar: establecer los valores para las variables uniform de la camara y de los samplers de las texturas.
	glUniform3fv(locUniformCamera, 1, glm::value_ptr(cameraPos));
	glUniform1i(locUniformStone, 0);  // GL_TEXTURE0
	glUniform1i(locUniformMoss, 1);   // GL_TEXTURE1
	glUniform1i(locUniformMap, 2);    // GL_TEXTURE2
	glUniform1i(locUniformRenderMode, renderMode);
	glUniform1i(locUniformChromatic, chromaticOn ? 1 : 0);
	glUniform1i(locUniformFresnel, fresnelOn ? 1 : 0);

	// Pasar índices de refracción (eta = aire / vidrio)
	glUniform1f(locUniformEtaR, etaAir / (etaGlassR * refractionFactor));
	glUniform1f(locUniformEtaG, etaAir / (etaGlassG * refractionFactor));
	glUniform1f(locUniformEtaB, etaAir / (etaGlassB * refractionFactor));

	float etaReal = etaAir / etaGlassG; // Usamos el verde para el cálculo de Fresnel
	float fresnelF = pow((1.0f - etaReal) / (1.0f + etaReal), 2.0f);
	glUniform1f(locUniformFresnelF, fresnelF);

	// Tarea a realizar (ejercicio 1.2b): descomentar el c�digo

	// Dibuja el entorno
	mvp = Projection * View * ModelSky;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelSky)));
	// Tarea a realizar (ejercicio 1.2b): pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, glm::value_ptr(ModelSky));
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, glm::value_ptr(nm));
	glUniform1i(locUniformDrawSky, 1);
	drawCube();
	glUniform1i(locUniformDrawSky, 0);

	// Dibujar Esfera
	mvp = Projection * View * ModelSphere;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelSphere)));
	// Tarea a realizar (ejercicio 1): pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, glm::value_ptr(ModelSphere));
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, glm::value_ptr(nm));
	drawSphere();

	// Tarea a realizar (ejercicio 1): Dibuja la tetera y el cubo
	mvp = Projection * View * ModelTeapot;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelTeapot)));
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, glm::value_ptr(ModelTeapot));
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, glm::value_ptr(nm));
	drawTeapot();

	// Cubo
	mvp = Projection * View * ModelCube;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelCube)));
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, glm::value_ptr(ModelCube));
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, glm::value_ptr(nm));
	drawCube();

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
		xrot += 0.03f;
		yrot += 0.04f;
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
	// Tarea a realizar (ejercicio 1.2): activar la tecla 'R'
	case 'r': case 'R':
		renderMode = (renderMode + 1) % 3;
		if (renderMode == 0)
			std::cout << "Modo: Texturas (1.1)" << std::endl;
		else if (renderMode == 1)
			std::cout << "Modo: Reflejo (1.2)" << std::endl;
		else
			std::cout << "Modo: Refraccion (1.3)" << std::endl;
		break;
	case 'c': case 'C':
		chromaticOn = !chromaticOn;
		std::cout << "Dispersion cromatica: " << (chromaticOn ? "ON" : "OFF") << std::endl;
		break;
	case 'f': case 'F':
		fresnelOn = !fresnelOn;
		std::cout << "Fresnel: " << (fresnelOn ? "ON" : "OFF") << std::endl;
		break;
	case '+':
		refractionFactor += 0.1f; // Aumenta el factor de refracción
		break;
	case '-':
		refractionFactor -= 0.1f; // Disminuye el factor de refracción
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