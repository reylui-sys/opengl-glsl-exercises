#version 150

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec3 aTangent;   // Tangente del vértice en el S.R. local al objeto
in vec2 aTexCoord;  // Coordenadas de textura

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
};
uniform LightInfo uLight;

// Ya no se usan los out de la posicion y la normal.

// Variables de salida hacia el fragment shader
out vec2 vTexCoord;
out vec3 vLightDir;   // Dirección de la luz en espacio tangente
out vec3 vViewDir;    // Dirección de la vista en espacio tangente

void main()
{
	vec3 ecPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0)); // Posición en el S.R. vista

	// Vector normal y tangeente pasado al espacio vista
	vec3 N = normalize(uNormalMatrix * aNormal);
	vec3 T = normalize(uNormalMatrix * aTangent);
	// Calcular bitangente (B) mediante producto vectorial
	vec3 B = normalize(cross(N, T));

	// Construir matriz TBN (transforma de espacio vista a espacio tangente)
	// La matriz TBN es la traspuesta de [T|B|N]. Queremos la traspuesta porque queremos transformar vectores del espacio vista al espacio tangente, no al revés.
	mat3 TBN = transpose(mat3(T, B, N)); // Matriz TBN (transforma de espacio vista a espacio tangente)

	vec3 lightDir = vec3(uLight.lightPos) - ecPos; // Vector luz en espacio vista
	vLightDir = TBN * lightDir; // Vector luz en espacio tangente
	
	vec3 viewDir = -ecPos; // Vector vista en espacio vista (desde el fragment al ojo)
	vViewDir = TBN * viewDir; // Vector vista en espacio tangente

	vTexCoord = aTexCoord; // Pasar coordenadas de textura
	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0); // Transformación de posición
}