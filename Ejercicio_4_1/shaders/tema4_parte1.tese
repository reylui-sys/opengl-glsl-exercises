#version 400

// quads
layout (quads) in;

// Color recibido del TCS (array)
in vec3 tcColor[];

// Variables de salida hacia el fragment shader
out vec3 vECPos;   // Posicion en S.R. vista
out vec3 vECNorm;  // Normal en S.R. vista
out vec3 vColor;   // Color

// Matrices
uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

void main()
{
	// Coordenadas parametricas del vertice generado por el TPG
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// Convertir coordenadas parametricas (u,v) a posicion en S.R. local del objeto (el S.R. de los puntos de control, que es el S.R. del cubo)
	// Interpolación bilinea en el eje x (u) y eje y (v) para saber el porcentaje del punto generado respecto a la esfera
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	vec4 pos = p0 + (p1 - p0) * u + (p3 - p0) * v;

	// Convertir el punto del cubo en un punto de la esfera: normalizando todo vec pos misma norma, radio de la esfera (como morph tetera)
	vec3 spherePos = normalize(pos.xyz);

	// La normal es la misma en referencia al modelo de la esfera
	vec3 normal = spherePos;

	// Funcion del "vertex", se proyecta el punto de la esfera a espacio de pantalla, y se pasa la posicion y normal en S.R. vista al fragment shader
	vECPos  = vec3(uModelViewMatrix * vec4(spherePos, 1.0));
	vECNorm = normalize(uNormalMatrix * normal);

	// El color se interpola bilinealmente desde los 4 puntos de control del parche
	vec3 c0 = tcColor[0];
	vec3 c1 = tcColor[1];
	vec3 c3 = tcColor[3];
	vColor = c0 + (c1 - c0) * u + (c3 - c0) * v;

	// Posicion final en espacio de pantalla
	gl_Position = uModelViewProjMatrix * vec4(spherePos, 1.0);
}
