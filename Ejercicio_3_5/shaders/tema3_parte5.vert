#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

uniform int uDrawingShadowMap;

// Matriz de transformación de coord. locales a coord. de textura de sombra
uniform mat4 uShadowMatrix;

out vec3 vECPos; // S.R. Vista
out vec3 vECNorm; // S.R. Vista

// Coordenadas proyectivas en el mapa de sombras
out vec4 vShadowCoord;

void main()
{
	vECPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
	vECNorm = normalize(uNormalMatrix * aNormal);

	// Transforma la posición local al espacio de la textura de sombra
	// uShadowMatrix = B * ProjectionLight * ViewLight * Model
	vShadowCoord = uShadowMatrix * vec4(aPosition, 1.0);

	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}