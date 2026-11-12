// Ejercicios tema 1 - parte 2 (PTG - 2026)

#include <GL/glut.h>
#include <iostream>
#include "lodepng.h" // Librería para cargar imágenes PNG.

// Prototipos de las funciones

GLboolean init();
void display();
void resize(GLint, GLint);
void idle();
void keyboard(GLubyte, GLint, GLint);
void specialKeyboard(GLint, GLint, GLint);
void mouse(GLint, GLint, GLint, GLint);
void mouseMotion(GLint, GLint);

// Variables globales con los estados globales de la aplicación

GLboolean fullscreen = false;
GLboolean mouseDown = false;
GLboolean animation = false;

GLfloat xrot = 0.0f;
GLfloat yrot = 0.0f;
GLfloat xdiff = 0.0f;
GLfloat ydiff = 0.0f;

GLint g_Width = 500; // Ancho inicial de la ventana
GLint g_Height = 500; // Altura incial de la ventana

GLboolean lightningEnabled = true; // Variable para activar/desactivar la iluminación

GLboolean sombreado_plano = false; // Variable para activar/desactivar el sombreado plano

GLint tipoLuz = 0; // 0 = puntual, 1 = direccional, 2 = foco
const GLint NUM_TIPOS_LUZ = 3;

GLboolean texturaActiva = false; // Variable para activar/desactivar la textura
GLuint textura_id; // ID del objeto textura. Se usa unsignado por OpenGL para referenciar a la textura cargada en memoria

GLint modoPegadoTextura = 0; // 0 - GL_REPLACE, 1 - GL_MODULATE
const GLint NUM_TIPOS_PEGADO = 2;
GLboolean modoRepeticion = false; // Variable para activar/desactivar el modo de repetición de la textura
GLint filtro = 0; // Variable para cambiar el filtro de la textura
const GLint NUM_FILTROS = 3;


GLint main(GLint argc, char *argv[])
{
	glutInit(&argc, (char **)argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
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

void drawPlane()
{
	const GLuint numDivisiones = 100;
	const GLfloat TAM = 3.0f;
	for (GLuint i = 0; i < numDivisiones; i ++ )
	{
		GLfloat y0 = TAM * GLfloat(i) / numDivisiones - TAM / 2.0f;
		GLfloat yf = TAM * GLfloat(i+1) / numDivisiones - TAM / 2.0f;
		glBegin(GL_TRIANGLE_STRIP);
		for ( GLint j = 0; j <= numDivisiones; j ++ )
		{
			GLfloat x = TAM * GLfloat(j) / numDivisiones - TAM / 2.0f;
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(x,y0);
			glVertex3f(x, y0, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(x,yf);
			glVertex3f(x, yf, 0.0f);
		}
		glEnd();
	}
	return;
}

GLboolean init()
{
	const GLfloat luzLocal[] = { 0.5f, 0.5f, 1.0f, 1.0f }; // w = 1.0f para luz puntual
	const GLfloat luzDireccional[] = { 0.5f, 0.5f, 1.0f, 0.0f }; // w = 0.0f para luz direccional (la dirección de la luz es la dirección del vector desde el origen a la posición de la luz)
	const GLfloat direccionFoco[] = { -0.5f, -0.5f, -3.0f }; // Dirección del foco (desde el origen hacia la dirección del foco)
	const GLfloat amplitudFoco = 10.0f; // En grados
	const GLfloat intensidadLuz[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color de la parte ambiente, difusa y especular de la luz.

	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	
	// Tarea por hacer: Activar la iluminación.
	// Ya no calcula solo con glColor4f, calcula el color de cada vértice con el modelo Phong
	glEnable(GL_LIGHTING); // Sin esto, glMaterialfv no tiene efecto sobre los objetos de la escena.
	
	// Tarea por hacer: definir 3 luces.

	// 1º luz: puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luzLocal); // Posición de la luz (x,y,z,w) con w = 1.0f para luz puntual
	glLightfv(GL_LIGHT0, GL_AMBIENT, intensidadLuz); // Color de la parte ambiente de la luz
	glLightfv(GL_LIGHT0, GL_DIFFUSE, intensidadLuz); // Color de la parte difusa de la luz
	glLightfv(GL_LIGHT0, GL_SPECULAR, intensidadLuz); // Color de la parte especular de la luz

	glEnable(GL_LIGHT0); // Activar la luz 0

	// 2º luz: direccional
	glLightfv(GL_LIGHT1, GL_POSITION, luzDireccional); // Posición de la luz (x,y,z,w) con w = 0.0f para luz direccional
	glLightfv(GL_LIGHT1, GL_AMBIENT, intensidadLuz); // Color de la parte ambiente de la luz
	glLightfv(GL_LIGHT1, GL_DIFFUSE, intensidadLuz); // Color de la parte difusa de la luz
	glLightfv(GL_LIGHT1, GL_SPECULAR, intensidadLuz); // Color de la parte especular de la luz
	// No se activa la luz 1, para que no afecte a la escena por defecto

	// 3º luz: foco
	glLightfv(GL_LIGHT2, GL_POSITION, luzLocal); // Posición del foco (x,y,z,w) con w = 1.0f para luz puntual
	glLightfv(GL_LIGHT2, GL_AMBIENT, intensidadLuz); // Color de la parte ambiente de la luz
	glLightfv(GL_LIGHT2, GL_DIFFUSE, intensidadLuz); // Color de la parte difusa de la luz
	glLightfv(GL_LIGHT2, GL_SPECULAR, intensidadLuz); // Color de la parte especular de la luz
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direccionFoco); // Dirección del foco (desde el origen hacia la dirección del foco)
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, amplitudFoco); // Ángulo de apertura del foco (en grados, entre 0 y 90, o 180 para luz sin foco)
	// No se activa la luz 2, para que no afecte a la escena por defecto
	

	// Tarea por hacer: Crear el obj. textura, cargar la imagen png, activar la textura, 
	// cargar los datos en el objeto textura y definir filtros, modo repetición y modo pegado

 	std::vector<GLubyte> img_data;
	GLuint img_width, img_height;

	// const char* img_filename = "texturas/Copper Rough_diffuse.png";
    // const char* img_filename = "../texturas/Craft_diffuse.png";
    // const char* img_filename = "../texturas/Fabric 02_diffuse.png";

	const char* img_filename = "C:\\PTG\\Ejercicio_1_2\\proyecto\\texturas\\Copper Rough_diffuse.png";
	//const char* img_filename = "C:\\PTG\\Ejercicio_1_2\\proyecto\\texturas\\Craft_diffuse.png";
    //const char* img_filename = "C:\\PTG\\Ejercicio_1_2\\proyecto\\texturas\\Fabric 02_diffuse.png";

	std::cout << "Cargando la textura " << img_filename << std::endl;
	GLuint error = lodepng::decode(img_data, img_width, img_height, img_filename);

	if ( !error )
    {	
		glGenTextures(1, &textura_id); // Generar el ID de la textura
		glBindTexture(GL_TEXTURE_2D, textura_id); // Activar la textura para cargar los datos
	
		//gDlTexImage2(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img_data[0]);
		gluBuild2DMipmaps(GL_TEXTURE_2D,			// texture to specify
				GL_RGBA,				// internal texture storage format
				img_width,			// texture width
				img_height,			// texture height
				GL_RGBA,				// pixel format
				GL_UNSIGNED_BYTE,	// color component format
				&img_data[0]);		// pointer to texture image

		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtro de magnificación
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Filtro de minificación
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // Modo de repetición en S
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // Modo de repetición en T
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // Modo de pegado de la textura
	   
		*/
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // Modo de repetición en S
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // Modo de pegado de la textura
		
		std::cout << "tamanyo de la textura (w,h) = (" << img_width << "," << img_height << ")" << std::endl;
		
	}
	else
	   std::cout << "Error al cargar la textura" << img_filename << std::endl;
	
	return true;
}
 
void display()
{
	/*
	El material define como la superficie refleja la luz.
	Cada componente del material (ambiente, difusa y especular) se define con un color RGBA
	y se multiplica con la componente de la luz
	*/
	GLfloat bronzeColorAmb[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat bronzeColorDiff[4] = { 0.8f, 0.6f, 0.0f, 1.0f };
	GLfloat bronzeColorSpec[4] = { 1.0f, 1.0f, 0.8f, 1.0f };
	GLfloat bronzeShininess = 50;
	GLfloat colorNone[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	gluLookAt(
	0.0f, 0.0f, 3.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	glColor4fv(bronzeColorDiff); // Color predefinidos de los objetos
	
	// Tarea por hacer: Activar el material antes de dibujar los objetos de la escena.
	// GL_FRONT_AND_BACK para que el material afecte a ambas caras de los objetos
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bronzeColorAmb); // Color de la parte ambiente del material
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bronzeColorDiff); // Color de la parte difusa del material
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bronzeColorSpec); // Color de la parte especular del material
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, bronzeShininess); // Brillo del material (cuanto más alto, más brillante)
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colorNone); // Color de la parte emisiva del material (si el material emite luz, no se ve afectado por la iluminación de la escena)
	
	// if (texturaActiva)
	// {
	// 	glEnable(GL_TEXTURE_2D); // Activar el uso de texturas 2D
	// 	glBindTexture(GL_TEXTURE_2D, textura_id); // Activar la textura para usarla en el dibujo de los objetos de la escena
	// }
	// else
	// 	glDisable(GL_TEXTURE_2D);

	// drawPlane();
	glutSolidTeapot(0.5f);
	//glutSolidCube(0.5f);
	//glutSolidSphere(0.5f, 20, 20);
	//glutSolidCone(0.25f, 0.5f, 20, 1);
	//glutSolidTorus(0.25, 0.5, 10, 20);

	glPopMatrix();
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
 
void keyboard(GLubyte key, GLint x, GLint y)
{
	static GLint tipoLuz = 0; // 0 - local, 1 - Direccional, 2- Foco
	const GLint NUM_TIPOS_LUZ = 3;
	static GLint modoPegadoTextura = 0; // 0 - GL_REPLACE, 1 - GL_MODULATE
	const GLint NUM_TIPOS_PEGADO = 2;
	static GLboolean  modoRepeticion = false;
	static GLint filtro = 0;

	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(EXIT_SUCCESS); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'l': case 'L':
		lightningEnabled = !lightningEnabled;
		if (lightningEnabled)
		{
			glEnable(GL_LIGHTING);
			std::cout << "Iluminación activada" << std::endl;
		}
		else
		{
			glDisable(GL_LIGHTING);
			std::cout << "Iluminación desactivada" << std::endl;
		}
	break;
	case 's': case 'S':
	/*
	glShadeModel controlaa la interpolación de color dentro del triángulo.
	GL_SMOOTH: cada pixel se colorea con el color interpolado de los vértices del triángulo.
	GL_FLAT: cada pixel se colorea con el color de un solo vértice del triángulo (el último vértice especificado). Se ven las caras indivduales, como un poliedro
	*/
		sombreado_plano = !sombreado_plano;
		if (sombreado_plano)
		{
			glShadeModel(GL_FLAT);
			std::cout << "Sombreado plano activado" << std::endl;
		}
		else
		{
			glShadeModel(GL_SMOOTH);
			std::cout << "Sombreado plano desactivado" << std::endl;
		}
	break;
	case 't': case 'T':
		tipoLuz = (tipoLuz + 1) % NUM_TIPOS_LUZ;
		// glDisable(GL_LIGHT0);
		// glDisable(GL_LIGHT1);
		// glDisable(GL_LIGHT2);
		switch (tipoLuz)
		{
		case 0:
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			std::cout << "Luz puntual activada" << std::endl;
			break;
		case 1:
			glDisable(GL_LIGHT0);
			glEnable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			std::cout << "Luz direccional activada" << std::endl;
			break;
		case 2:
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glEnable(GL_LIGHT2);
			std::cout << "Foco activado" << std::endl;
			break;
		default:
			break;
		}
	break;
	case 'x': case 'X':
		texturaActiva = !texturaActiva;
		if (texturaActiva)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textura_id); // Activar la textura para usarla en el dibujo de los objetos de la escena
			std::cout << "Textura activada" << std::endl;
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			std::cout << "Textura desactivada" << std::endl;
		}
	break;
	case 'p': case 'P':
		modoPegadoTextura = (modoPegadoTextura + 1) % NUM_TIPOS_PEGADO;
		switch (modoPegadoTextura)
		{
		case 0:
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			std::cout << "Modo pegado de la textura: GL_REPLACE" << std::endl;
			break;
		case 1:
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			std::cout << "Modo pegado de la textura: GL_MODULATE" << std::endl;
			break;
		}
	break;
	case 'e': case 'E':
		modoRepeticion = !modoRepeticion;
		if (modoRepeticion)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			std::cout << "Modo de repetición de la textura: GL_REPEAT" << std::endl;
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			std::cout << "Modo de repetición de la textura: GL_CLAMP" << std::endl;
		}
	break;
	case 'f': case 'F':
		filtro = (filtro + 1) % NUM_FILTROS;
		switch (filtro)
		{
		case 0:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			std::cout << "Filtro de la textura: GL_NEAREST" << std::endl;
			break;
		case 1:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			std::cout << "Filtro de la textura: GL_LINEAR" << std::endl;
			break;
		case 2:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			std::cout << "Filtro de la textura: GL_LINEAR_MIPMAP_LINEAR" << std::endl;
			break;
		default:
			break;
		}
	break;
	case 'm': case 'M':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		std::cout << "Modo de dibujo: GL_LINE" << std::endl;
		break;
	case 'n': case 'N':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		std::cout << "Modo de dibujo: GL_FILL" << std::endl;
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
