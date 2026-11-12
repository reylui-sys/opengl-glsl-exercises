// Ejercicios del tema 1 - parte 3 (PTG - 2026)
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "lodepng.h"

void printFPS();

void draw1Cube();
void drawMultipleCubes();
// Ejercicio 3.2 - Vertex Arrays
void drawMultipleCubesVA1();
void drawMultipleCubesVA2();
void drawMultipleCubesVA3();
// Ejercicio 3.3 - Vertex Buffer Objects
void initVBOs();
void drawMultipleCubesVBO4();
void drawMultipleCubesVBO5();
void drawMultipleCubesVBO6();
// Ejercicio 3.4 - Vertex Array Objects
void initVAOs();
void drawMultipleCubesVAO7();
void drawMultipleCubesVAO8();
void drawMultipleCubesVAO9();

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

GLboolean textureOn = false;
GLuint textura_id;
GLuint opcion = 0;

// IDs de VBOs (ejercicio 3.3)
GLuint vboVertices1, vboColores1, vboTexcoord1;              // Variante 4
GLuint vboVertices2, vboColores2, vboTexcoord2, vboIndices2; // Variante 5
GLuint vboVertices3, vboIndices3;                            // Variante 6

// IDs de VAOs (ejercicio 3.4)
GLuint vao7, vao8, vao9;

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
// vertice tiene 3 components (x,y,z) de reales, por tanto, el tamaño del vertex
// array es 108 floats (36 * 3 = 108).
const GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
                       -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
                        1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
                       -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
                       -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

                       -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
                       -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4
// color array
const GLfloat colores1[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
                        1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
                        0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0

                        1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
                        0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1

                        0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
                        1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7

                        0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
                        0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4
// texture coord. array
const GLfloat texcoord1[] = { 1, 1,   0, 1,  0, 0,      // v0-v1-v2 (front)
						0, 0,   1, 0,  1, 1,      // v2-v3-v0

						1, 1,   0, 1,   0, 0,      // v0-v3-v4 (right)
						0, 0,   1, 0,   1, 1,      // v4-v5-v0

						1, 1,   1, 0,   0, 0,      // v0-v5-v6 (top)
						0, 0,   0, 1,   1, 1,      // v6-v1-v0

					    1, 1,   1, 0,   0, 0,      // v1-v6-v7 (left)
						0, 0,   0, 1,   1, 1,      // v7-v2-v1

						0, 0,   1, 0,   1, 1,      // v7-v4-v3 (bottom)
						1, 1,   0, 1,   0, 0,      // v3-v2-v7

						1, 0,   0, 0,   0, 1,      // v4-v7-v6 (back)
						0, 1,   1, 1,   1, 0 };    // v6-v5-v4


// Coordenadas del vertex array eliminando los vértices repetidos (más compacto)
const GLfloat vertices2[]= { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
						1,-1,-1,   1, 1,-1,  -1, 1,-1,  -1,-1,-1};	// v4,v5,v6,v7 (back)

// color array
const GLfloat colores2[]  = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)0
						0, 0, 1,   0, 1, 1,   0, 1, 0,   0, 0, 0};	// v4,v5,v6,v7 (back)

// texture coord. array
const GLfloat texcoord2[] = { 1, 1,   0, 1,   0, 0,   1, 0,		// v0,v1,v2,v3 (front)
						1, 1,   1, 0,   0, 0,   0, 1 };		// v4,v5,v6,v7 (back)

// index array del vertex array
const GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
                       0, 3, 4,   4, 5, 0,      // right
                       0, 5, 6,   6, 1, 0,      // top
                       1, 6, 7,   7, 2, 1,      // left
                       7, 4, 3,   3, 2, 7,      // bottom
                       4, 7, 6,   6, 5, 4 };    // back

// Vertex array entrelazado
// Los dos atributos (posición y color) se emaquetan juntos. EL resultado es 
// algo similar a un conjunto de estructuras: ((V,C,T), (V,C,T), (V,C,T),...).
const GLfloat vertices3[]= {	 1, 1, 1,  1, 1, 1,	 1, 1,	// v0
						-1, 1, 1,  1, 1, 0,	 0, 1,	// v1
						-1,-1, 1,  1, 0, 0,	 0, 0,	// v2
						 1,-1, 1,  1, 0, 1,	 1, 0,	// v3
						 1,-1,-1,  0, 0, 1,	 1, 1,	// v4
						 1, 1,-1,  0, 1, 1,	 1, 0,	// v5
						-1, 1,-1,  0, 1, 0,	 0, 0,	// v6
						-1,-1,-1,  0, 0, 0,  0, 1};	// v7


int main(int argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tema 1, parte 3");
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

// dibuja 1 cubo: modo inmediato
// 108 llamadas = 36 glVertex*() + 36 glColor*() + 36 glTexCoord*()
void draw1Cube()
{
    glBegin(GL_TRIANGLES);
		// front face
        // triangle v0-v1-v2
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
		glColor3f(1, 1, 0); glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
		glColor3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
        // triangle v2-v3-v0
		glColor3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
		glColor3f(1, 0, 1); glTexCoord2f(1, 0); glVertex3f(1, -1, 1);
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
        // right face
        // triangle v0-v3-v4
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
		glColor3f(1, 0, 1); glTexCoord2f(0, 1); glVertex3f(1, -1, 1);
		glColor3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(1, -1, -1);
        // triangle v4-v5-v0
		glColor3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(1, -1, -1);
		glColor3f(0, 1, 1); glTexCoord2f(1, 0); glVertex3f(1, 1, -1);
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
        // top face
        // triangle v0-v5-v6
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
		glColor3f(0, 1, 1); glTexCoord2f(1, 0); glVertex3f(1, 1, -1);
		glColor3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-1, 1, -1);
        // triangle v6-v1-v0
		glColor3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-1, 1, -1);
		glColor3f(1, 1, 0); glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
		glColor3f(1, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
        // left face
        // triangle  v1-v6-v7
		glColor3f(1, 1, 0); glTexCoord2f(1, 1); glVertex3f(-1, 1, 1);
		glColor3f(0, 1, 0); glTexCoord2f(1, 0); glVertex3f(-1, 1, -1);
		glColor3f(0, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
        // triangle v7-v2-v1
		glColor3f(0, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
		glColor3f(1, 0, 0); glTexCoord2f(0, 1); glVertex3f(-1, -1, 1);
		glColor3f(1, 1, 0); glTexCoord2f(1, 1); glVertex3f(-1, 1, 1);
        // bottom face
        // triangle v7-v4-v3
		glColor3f(0, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
		glColor3f(0, 0, 1); glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
		glColor3f(1, 0, 1); glTexCoord2f(1, 1); glVertex3f(1, -1, 1);
        // triangle v3-v2-v7
		glColor3f(1, 0, 1); glTexCoord2f(1, 1); glVertex3f(1, -1, 1);
		glColor3f(1, 0, 0); glTexCoord2f(0, 1); glVertex3f(-1, -1, 1);
		glColor3f(0, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
        // back face
        // triangle v4-v7-v6
		glColor3f(0, 0, 1); glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
		glColor3f(0, 0, 0); glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
		glColor3f(0, 1, 0); glTexCoord2f(0, 1); glVertex3f(-1, 1, -1);
        // triangle v6-v5-v4
		glColor3f(0, 1, 0); glTexCoord2f(0, 1); glVertex3f(-1, 1, -1);
		glColor3f(0, 1, 1); glTexCoord2f(1, 1); glVertex3f(1, 1, -1);
		glColor3f(0, 0, 1); glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
	glEnd();
	return;
}

// OPCIÓN 0: modo inmediato (glBegin/glEnd)
void drawMultipleCubes()
{
	const GLint N = 40; // Número de cubos en cada eje
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	// Desvincular VBOs y VAOs que puedan haber quedado activos de otros modos
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					draw1Cube();  // Si no se utiliza VA, VBO ni VAO
					// glDrawArrays o glDrawElements si se utiliza VA, VBO ni VAO
				glPopMatrix();
			}
		}
	}
	return;
}

// Ejercicio 3.2
// Tres versiones de drawMultipleCubes usando Vertex Arrays.
// Los datos están en arrays globales en RAM. En vez de enviar cada vértice
// con glVertex3f, se le dice a OpenGL "aquí tienes el array entero" y se
// dibuja con una sola llamada a glDrawArrays o glDrawElements.

// OPCIÓN 1: VA con tres arrays separados (vertices1, colores1, texcoord1)
void drawMultipleCubesVA1()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	// Desvincular VBOs que puedan haber quedado activos de otros modos.
	// Sin esto, glVertexPointer interpreta el puntero como offset en el VBO
	// en vez de como dirección de memoria RAM.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Configurar UNA vez antes del bucle
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices1);
	glColorPointer(3, GL_FLOAT, 0, colores1);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord1);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				glPopMatrix();
			}
		}
	}

	// Limpiar UNA vez después del bucle
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// OPCIÓN 2: VA con arrays compactos + array de índices (vertices2, colores2, texcoord2, indices)
void drawMultipleCubesVA2()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	// Desvincular VBOs que puedan haber quedado activos de otros modos
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices2);
	glColorPointer(3, GL_FLOAT, 0, colores2);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord2);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
				glPopMatrix();
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// OPCIÓN 3: VA con array entrelazado + array de índices (vertices3, indices)
void drawMultipleCubesVA3()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	const GLsizei stride = 8 * sizeof(GLfloat); // 3 pos + 3 col + 2 tex = 8 floats = 32 bytes

	// Desvincular VBOs que puedan haber quedado activos de otros modos
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, stride, vertices3);
	glColorPointer(3, GL_FLOAT, stride, vertices3 + 3);
	glTexCoordPointer(2, GL_FLOAT, stride, vertices3 + 6);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
				glPopMatrix();
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// Ejercicio 3.3 - Vertex Buffer Objects (VBO)
// Los datos se suben a la memoria de la GPU (VRAM) una sola vez en init().
// Al dibujar, la GPU lee directamente de su propia memoria
void initVBOs()
{
	// VBOs para variante 4 (arrays separados sin índices)
	glGenBuffers(1, &vboVertices1);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glGenBuffers(1, &vboColores1);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colores1), colores1, GL_STATIC_DRAW);

	glGenBuffers(1, &vboTexcoord1);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord1), texcoord1, GL_STATIC_DRAW);

	// VBOs para variante 5 (arrays compactos + índices)
	glGenBuffers(1, &vboVertices2);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glGenBuffers(1, &vboColores2);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colores2), colores2, GL_STATIC_DRAW);

	glGenBuffers(1, &vboTexcoord2);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord2), texcoord2, GL_STATIC_DRAW);

	glGenBuffers(1, &vboIndices2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// VBOs para variante 6 (entrelazado + índices)
	glGenBuffers(1, &vboVertices3);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);

	glGenBuffers(1, &vboIndices3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Desvincular buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// OPCIÓN 4: VBO con arrays separados, sin índices
void drawMultipleCubesVBO4()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertices1);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores1);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord1);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				glPopMatrix();
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// OPCIÓN 5: VBO con arrays compactos + índices
void drawMultipleCubesVBO5()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertices2);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores2);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord2);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices2);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
				glPopMatrix();
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// OPCIÓN 6: VBO con array entrelazado + índices
void drawMultipleCubesVBO6()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	const GLsizei stride = 8 * sizeof(GLfloat);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertices3);
	glVertexPointer(3, GL_FLOAT, stride, 0);
	glColorPointer(3, GL_FLOAT, stride, (GLvoid*)(3 * sizeof(GLfloat)));
	glTexCoordPointer(2, GL_FLOAT, stride, (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices3);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
				glPopMatrix();
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
}

// Ejercicio 3.4 - Vertex Array Objects (VAO)
// Un VAO "graba" toda la configuración de VBOs + punteros + enables.
// Se configuran UNA vez en init, y al dibujar solo se hace glBindVertexArray.
void initVAOs()
{
	// VAO 7: arrays separados sin índices (usa VBOs de variante 4)
	glGenVertexArrays(1, &vao7);
	glBindVertexArray(vao7);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices1);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores1);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord1);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindVertexArray(0);

	// VAO 8: arrays compactos + índices (usa VBOs de variante 5)
	glGenVertexArrays(1, &vao8);
	glBindVertexArray(vao8);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices2);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboColores2);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord2);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices2);
	glBindVertexArray(0);

	// VAO 9: entrelazado + índices (usa VBOs de variante 6)
	glGenVertexArrays(1, &vao9);
	glBindVertexArray(vao9);
	const GLsizei stride = 8 * sizeof(GLfloat);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices3);
	glVertexPointer(3, GL_FLOAT, stride, 0);
	glColorPointer(3, GL_FLOAT, stride, (GLvoid*)(3 * sizeof(GLfloat)));
	glTexCoordPointer(2, GL_FLOAT, stride, (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices3);
	glBindVertexArray(0);
}

// OPCIÓN 7: VAO con arrays separados
void drawMultipleCubesVAO7()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vao7);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				glPopMatrix();
			}
		}
	}

	glBindVertexArray(0);
	return;
}

// OPCIÓN 8: VAO con arrays compactos + índices
void drawMultipleCubesVAO8()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vao8);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
				glPopMatrix();
			}
		}
	}

	glBindVertexArray(0);
	return;
}

// OPCIÓN 9: VAO con array entrelazado + índices
void drawMultipleCubesVAO9()
{
	const GLint N = 40;
	const GLfloat RangoIni = -1.0f;
	const GLfloat RangoFin =  1.0f;
	GLfloat x, y, z;
	GLfloat scale = (RangoFin - RangoIni) / (N * 3);

	glBindVertexArray(vao9);

	for ( GLint i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( GLint j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( GLint k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
				glPopMatrix();
			}
		}
	}

	glBindVertexArray(0);
	return;
}

GLboolean init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	// Ejercicio 3.1: Carga e inicializa la textura
	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;
	const char* img_filename = "../texturas/wood_box.png"; //  En modo debug no va, necesita las optimizaciones.
	//const char* img_filename = "C:\\PTG\\Ejercicio_1_3\\proyecto\\texturas\\wood_box.png";

	std::cout << "Cargando la textura " << img_filename << std::endl;
	unsigned int error = lodepng::decode(img_data, img_width, img_height, img_filename);

	if (!error)
	{
		glGenTextures(1, &textura_id);
		glBindTexture(GL_TEXTURE_2D, textura_id);

		gluBuild2DMipmaps(GL_TEXTURE_2D,			// texture to specify
			GL_RGBA,				// internal texture storage format
			img_width,			// texture width
			img_height,			// texture height
			GL_RGBA,				// pixel format
			GL_UNSIGNED_BYTE,	// color component format
			&img_data[0]);		// pointer to texture image

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		std::cout << "Textura cargada: " << img_width << "x" << img_height << std::endl;
	}
	else
	{
		std::cout << "Error al cargar la textura " << img_filename << std::endl;
	}

	// Ejercicio 3.3: Crear VBOs (subir datos a la GPU)
	initVBOs();

	// Ejercicio 3.4: Crear VAOs (grabar configuración)
	initVAOs();

	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(
	0.0f, 0.0f, 5.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	// Ejercicio 3.1: Activa/desactiva textura
	if (textureOn)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textura_id);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	// Seleccionar modo de dibujado según la opción
	switch (opcion)
	{
		case 0: drawMultipleCubes(); break; // Modo inmediato
		case 1: drawMultipleCubesVA1(); break; // VA arrays separados
		case 2: drawMultipleCubesVA2(); break; // VA arrays + índices
		case 3: drawMultipleCubesVA3(); break; // VA entrelazado + índices
		case 4: drawMultipleCubesVBO4(); break; // VBO arrays separados
		case 5: drawMultipleCubesVBO5(); break; // VBO arrays + índices
		case 6: drawMultipleCubesVBO6(); break; // VBO entrelazado + índices
		case 7: drawMultipleCubesVAO7(); break; // VAO arrays separados
		case 8: drawMultipleCubesVAO8(); break; // VAO arrays + índices
		case 9: drawMultipleCubesVAO9(); break; // VAO entrelazado + índices
	}

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
 
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27 : case 'q': case 'Q':
			exit(EXIT_SUCCESS); 
			break;
		case 'a': case 'A':
			animation = !animation;
			break;
		case 'x': case 'X': // Activa/desactiva textura
			textureOn = !textureOn;
			std::cout << "Textura " << (textureOn ? "ON" : "OFF") << std::endl;
			break;
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
			opcion = key - '0';
			const char* nombres[] = {
				"0: Inmediato (glBegin/glEnd)",
				"1: VA - arrays separados",
				"2: VA - arrays + indices",
				"3: VA - entrelazado + indices",
				"4: VBO - arrays separados",
				"5: VBO - arrays + indices",
				"6: VBO - entrelazado + indices",
				"7: VAO - arrays separados",
				"8: VAO - arrays + indices",
				"9: VAO - entrelazado + indices"
			};
			std::cout << "Modo: " << nombres[opcion] << std::endl;
			break;
	}
	return;
}
 
void specialKeyboard(int key, int x, int y)
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
 
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;
 
		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}
 
void mouseMotion(int x, int y)
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
	static int frameCount = 0;			//  Número de frames
	static float fps = 0;
	static int currentTime = 0, previousTime = 0;

	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);

	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);

		previousTime = currentTime;

		frameCount = 0;

		std::cout.precision(4);
		std::cout << "FPS: " << fps << std::endl;
	}
}